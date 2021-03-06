#include <vector>
#include <string>
#include <string_view>
#include <fstream>
#include <iostream>
#include <functional>
#include <future>
#include <thread>
#include <chrono>

#include <boost/filesystem.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm/transform.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/regex.h>

using namespace boost::adaptors;
using namespace boost::filesystem;

#include "input_processing.h"

int main(int argc, char** argv) {
    try {
        //getting input from console arguments
        std::vector<std::string_view> cmd_args(argv, argv + argc);
        auto const path = get_from_cmd<std::string_view>(cmd_args, "-p", "path");
        auto const mask = get_from_cmd<std::string_view>(cmd_args, "-m", "file mask");
        auto substring_stream = get_from_cmd<std::basic_ifstream<uint8_t>>(cmd_args, "-i", "substring file");
        auto ou_stream = get_from_cmd<std::ofstream>(cmd_args, "-o", "result file");

        const std::vector<uint8_t> substring{ std::istreambuf_iterator<uint8_t>(substring_stream), std::istreambuf_iterator<uint8_t>() };//unfortunately very inefficient
 
        boost::regex const my_filter(quick_fix(mask.data()));
        boost::smatch what;
        boost::for_each(
            recursive_directory_iterator(path.data())
            | filtered(static_cast<bool(*)(const boost::filesystem::path &)>(&is_regular_file))
            | filtered(
				[&what, &my_filter](const boost::filesystem::path &path) { 
					return boost::regex_match(path.filename().string(), what, my_filter); 
				})//could've used function binding here
			| transformed(
				[&substring](const boost::filesystem::path &path) { 
					return std::async(std::launch::deferred, [&](const boost::filesystem::path& path) { return std::wstring(has_substring(substring, path) ? path.c_str() : L""); }, path); 
				}),

            [&](std::future<std::wstring>& filename) {
                auto value = filename.get();
                if (!value.empty())
                    ou_stream << value << '\n';
            }
        );
    }
    catch (std::exception& e) {
        std::cout << "Could not do stuff. More information: " << e.what() << std::endl;
    }
}