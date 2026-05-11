#pragma once

#include <string>
#include <vector>

namespace file_utils {

std::string read_file(const std::string& path);
bool file_exists(const std::string& path);
std::string extension(const std::string& path);
std::string join_paths(const std::string& a, const std::string& b);
std::string base_name(const std::string& path);
std::string directory(const std::string& path);

}
