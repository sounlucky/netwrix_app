#include "file_processing.h"

void process_directory(std::wstring directory_path, std::wstring file_mask, const matcher_t& matcher) noexcept {
	WIN32_FIND_DATAW find_file_data;
	auto find_firstfileW_request = directory_path + file_mask;
	auto h_find = FindFirstFileW(find_firstfileW_request.data(), &find_file_data);
	// ignore unavailiable or empty directories
	if (h_find == INVALID_HANDLE_VALUE) {
		auto e = GetLastError();
		if (e != ERROR_FILE_NOT_FOUND) {
			std::wcerr << L"INVALID_HANDLE_VALUE from FindFirstFileW call processing directory " << directory_path << L", GetLastError : " << e << std::endl;
		}
		return;
	}
	do {
		// filter files only
		if (!(find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			auto file_fullname = directory_path + find_file_data.cFileName;
			try {
       			file_reader target_reader(file_fullname);// constructor must be called here and not inside lambda because it may throw runtime errors
				result_manager::instance().insert(
					[local_target_reader = std::move(target_reader), &matcher]() {
						if (matcher.find(local_target_reader)) {
							ou_stream::forward(local_target_reader.filename());
						}
					}
				);
			}
			catch (std::exception& e) {
				std::wcerr << L"Bad file " << find_file_data.cFileName << L", " << e.what() << L", GetLastError : " << GetLastError() << std::endl;
				continue;
			}
		}
	} while (FindNextFileW(h_find, &find_file_data));
	FindClose(h_find);
	auto e = GetLastError();
	if (e != ERROR_NO_MORE_FILES) {
		std::wcerr << L"Weird, getting GetLastError : " << e << L" != ERROR_NO_MORE_FILES during processing directory " << directory_path << std::endl;
	}
}