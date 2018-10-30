#pragma once
#include <array>
#include <vector>

#include "file_reader.h"


// class does not copy substring, thus attention to its lifetime is required,
// find method relies on valid pointer
class boyer_moore_matcher
{
	static constexpr auto table_size = 256;//there are 256 different uint8_t symbols
	std::array<std::vector<size_t>, table_size> lookup_badchar_table;
	const file_reader& substring;

	std::array<std::vector<size_t>, table_size> create_lookup_table(const file_reader& substring);
public:
	boyer_moore_matcher(const file_reader& substring);

	boyer_moore_matcher(const boyer_moore_matcher&) = delete;
	boyer_moore_matcher& operator=(const boyer_moore_matcher&) = delete;

	bool find(const file_reader&) const;
};

using matcher_t = boyer_moore_matcher;