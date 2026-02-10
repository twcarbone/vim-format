#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace vf
{

std::string read_file(const std::filesystem::path& path);

inline bool is_one_of(std::string_view str, std::string_view any)
{
    return str.size() == 1 && str.find_first_of(any) != std::string_view::npos;
}

};
