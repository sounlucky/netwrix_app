void custom_runtime_assert(bool value, std::wstring_view error_message) {
	if (!value) {
		std::mbstate_t state{};
		std::string char_msg;
		for (wchar_t wc : error_message) {
			std::string mb(MB_CUR_MAX, '\0');
			std::wcrtomb(&mb[0], wc, &state);
			char_msg += mb;
		}
		throw std::runtime_error(char_msg.data());
	}
}

//get string
template <>
struct get_from_cmd<const wchar_t*> {
	const wchar_t* operator()(std::vector<std::wstring_view>& source, std::wstring_view option, std::wstring_view name) {
		auto option_iter = std::find(source.cbegin(), source.cend(), option);
		custom_runtime_assert(option_iter != source.cend() && (option_iter + 1) != source.cend(), L"parameter \""s + name.data() + L"\" is not specified");
		return (option_iter + 1)->data();
	}
};

//get ifstream/ofstream
template <>
struct get_from_cmd<std::basic_ifstream<uint8_t>> {
		std::basic_ifstream<uint8_t> operator()(std::vector<std::wstring_view>& source, std::wstring_view option, std::wstring_view name) {
		auto filename = get_from_cmd<const wchar_t*>{}(source, option, name);
		std::basic_ifstream<uint8_t> stream(filename);
		custom_runtime_assert(stream.is_open(), L"could not open "s + filename);
		return stream;
	}
};
template <>
struct get_from_cmd<std::wofstream> {
	std::wofstream operator()(std::vector<std::wstring_view>& source, std::wstring_view option, std::wstring_view name) {
		auto filename = get_from_cmd<const wchar_t*>{}(source, option, name);
		std::wofstream stream(filename);
		custom_runtime_assert(stream.is_open(), L"could not open "s + filename);
		return stream;
	}
};
template <>
struct get_from_cmd<file_reader> {
	file_reader operator()(std::vector<std::wstring_view>& source, std::wstring_view option, std::wstring_view name) {
		auto filename = get_from_cmd<const wchar_t*>{}(source, option, name);
		return { filename };
	}
};