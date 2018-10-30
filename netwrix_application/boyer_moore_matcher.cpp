#include "boyer_moore_matcher.h"

boyer_moore_matcher::boyer_moore_matcher(const file_reader& substring) : lookup_badchar_table(create_lookup_table(substring)), substring(substring) {}

std::array<std::vector<size_t>, boyer_moore_matcher::table_size> boyer_moore_matcher::create_lookup_table(const file_reader& substring) {
	std::array<std::vector<size_t>, table_size> res;
	std::fill(std::begin(res), std::end(res), std::vector<size_t>(substring.size()));
	for (uint16_t c = 0; c < res.size(); ++c) {
		size_t shift_value = 1;
		for (size_t i = 0; i < substring.size(); ++i) {
			res[c][i] = shift_value;
			shift_value = c == substring[i] ? 1 : shift_value + 1;
		}
	}
	return res;
}

bool boyer_moore_matcher::find(const file_reader& input) const {
	if (input.size() < substring.size()) {
		return false;
	}
	size_t i = 0;
	while (i <= input.size() - substring.size()) {
		//find mismatch, mismatch_i is a possible index inside a substring
		size_t lastmatch_i = substring.size() - 1;
		uint8_t c;
		while ((c = input[i + lastmatch_i]) == substring[lastmatch_i]) {
			if (lastmatch_i == 0)
				break;
			--lastmatch_i;
		}
		if (lastmatch_i == 0)
			return true;
		//skip as much as possible
		size_t skip = lookup_badchar_table[c][lastmatch_i];
		i += skip;
	}
	return false;
}
