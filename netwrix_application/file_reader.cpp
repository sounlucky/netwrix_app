#include "file_reader.h"

// helper
std::string wstr_to_str(std::wstring_view target) {
	std::mbstate_t state{};
	std::string result;
	for (wchar_t wc : target) {
		std::string mb(MB_CUR_MAX, '\0');
		std::wcrtomb(&mb[0], wc, &state);
		result += mb;
	}
	return result;
}

const DWORD file_reader::alloc_granularity = [](){
	SYSTEM_INFO sys_info;
	GetSystemInfo(&sys_info);
	return sys_info.dwAllocationGranularity;
}();

file_reader::file_reader(std::wstring_view filename) :
	data(nullptr, &UnmapViewOfFile), file_handle(nullptr, &CloseHandle), mapping_handle(nullptr, &CloseHandle), filename_(filename), viewmap_start(0), viewmap_size(0) {
	file_handle.reset(CreateFileW(filename.data(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
	if (file_handle.get() == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("could not open file \'" + wstr_to_str(filename) + '\'');
	}
	file_size = GetFileSize(file_handle.get(), nullptr);
	mapping_handle.reset(CreateFileMapping(file_handle.get(), 0, PAGE_READONLY, 0, 0, 0));
	if (mapping_handle.get() == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("could not create mapping \'" + wstr_to_str(filename) + '\'');
	}
}

file_reader::file_reader(file_reader&& other) noexcept : 
	data(std::move(other.data)), 
	file_handle(std::move(other.file_handle)),  
	mapping_handle(std::move(other.mapping_handle)),
	file_size(other.file_size),
	viewmap_start(other.viewmap_start),
	viewmap_size(other.viewmap_size),
	filename_(std::move(other.filename_)),
	mutex_()
{
	other.data = nullptr;
	other.file_handle = nullptr;
	other.mapping_handle = nullptr;
	other.file_size = 0;
	other.viewmap_start = 0;
	other.viewmap_size = 0;
	other.filename_ = L"";
}

uint8_t file_reader::operator[](size_t index) const {
	{
		std::shared_lock lock(mutex_);
		if (viewmap_start <= index && index < viewmap_start + viewmap_size) {
			return data.get()[index - viewmap_start];
		}
	}
	// cache miss, safely reset viewmap
	std::unique_lock lock(mutex_);
	viewmap_start = (index / alloc_granularity) * alloc_granularity;
	viewmap_size = std::min<size_t>(alloc_granularity, size() - viewmap_start);
	data.reset(reinterpret_cast<uint8_t*>(MapViewOfFile(mapping_handle.get(), FILE_MAP_READ, 0, viewmap_start, viewmap_size)));
	return data.get()[index - viewmap_start];
}

file_reader::size_type file_reader::size() const {
	return file_size;
}

const wchar_t* file_reader::filename() const {
	return filename_.data();
}
