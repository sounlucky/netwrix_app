#pragma once
#include <string>
#include <string_view>
#include <fstream>
#include <future>
#include <iostream>
#include <type_traits>
#include <ios>

using namespace std::string_literals;

void custom_runtime_assert(bool value, std::wstring_view error_message);

template <typename T>
struct get_from_cmd {
	get_from_cmd() {}
	T operator()(std::vector<std::wstring_view>& source, std::wstring_view option, std::wstring_view name);
};

#include "input_processing.hpp"