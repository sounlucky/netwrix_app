#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <shared_mutex>

#define NOMINMAX
#include <Windows.h>

class file_reader
{
public:
	using size_type = DWORD;

	file_reader(std::wstring_view);
	file_reader(file_reader&&) noexcept;

	uint8_t operator[](size_t) const;
	size_type size() const;
	const wchar_t* filename() const;

private:
	std::unique_ptr<uint8_t, decltype(&UnmapViewOfFile)> mutable data;
	std::unique_ptr<void, decltype(&CloseHandle)> file_handle;
	std::unique_ptr<void, decltype(&CloseHandle)> mapping_handle;
	size_type file_size;
	const DWORD static alloc_granularity;
	mutable size_type viewmap_start;
	mutable size_type viewmap_size;
	std::wstring filename_;
	mutable std::shared_mutex mutex_;
};
