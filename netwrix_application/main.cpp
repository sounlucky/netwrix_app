#include <vector>
#include <queue>
#include <iostream>
#include <string_view>
#include <iterator>

#include "file_reader.h"
#include "input_processing.h"
#include "file_processing.h"

int wmain(int argc, wchar_t** argv) {
	// getting input from console arguments
	std::vector<std::wstring_view> cmd_args(argv, argv + argc);
	std::wstring const input_path = get_from_cmd<const wchar_t*>{}(cmd_args, L"-p", L"path");
	std::wstring const mask = get_from_cmd<const wchar_t*>{}(cmd_args, L"-m", L"file mask");
	file_reader substring(get_from_cmd<const wchar_t*>{}(cmd_args, L"-i", L"substring file"));
	ou_stream::init(get_from_cmd<const wchar_t*>{}(cmd_args, L"-o", L"result file"));
 	matcher_t matcher{ substring };// substring - finder needs substring once 
	std::queue<std::wstring> folders({ input_path.back() ==  L'\\' ? input_path : input_path + L'\\' });// so ".../FolderName/" and ".../FolderName" both work
	do {
		auto path = folders.front();
		folders.pop(); 
		process_directory(path, mask, matcher);
		// checking directory for subdirectories and process each one using function from file_processing.h
		WIN32_FIND_DATAW find_file_data;
		auto h_find = FindFirstFileW((path + L'*').data(), &find_file_data);
		if (h_find == INVALID_HANDLE_VALUE) {
			std::wcerr << L"Got INVALID_HANDLE_VALUE from FindFirstFileW in " << path << L", GetLastError : " << GetLastError() << std::endl;
		}
		do {
			if (find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
				if (_wcsicmp(find_file_data.cFileName, L".") && _wcsicmp(find_file_data.cFileName, L"..")){
					folders.emplace(path + find_file_data.cFileName + L'\\');
				}
			}
		} while (FindNextFileW(h_find, &find_file_data));
		FindClose(h_find);
		auto e = GetLastError();
		if (e != ERROR_NO_MORE_FILES) {
			std::wcerr << L"Weird FindNextFileW call result, GetLastError : " << e << L" != ERROR_NO_MORE_FILES in " << find_file_data.cFileName << std::endl;
		}
	} while (!folders.empty());
	result_manager::instance().wait();
}