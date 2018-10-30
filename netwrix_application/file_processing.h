#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <future>
#include <optional>
#include <iostream>
#include <array>

#include "boyer_moore_matcher.h"
#include "result_manager.h"
#include "ou_stream.h"

using filedata_t = std::vector<uint8_t>;

void process_directory(std::wstring directory_path, std::wstring file_mask, const matcher_t& matcher) noexcept ;
