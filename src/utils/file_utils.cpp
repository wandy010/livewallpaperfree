#include "file_utils.h"

#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace file_utils {

std::string read_file(const std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return {};
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

bool file_exists(const std::string& path) {
    return fs::exists(path);
}

std::string extension(const std::string& path) {
    return fs::path(path).extension().string();
}

std::string join_paths(const std::string& a, const std::string& b) {
    if (a.empty()) return b;
    if (b.empty()) return a;
    return (fs::path(a) / fs::path(b)).string();
}

std::string base_name(const std::string& path) {
    return fs::path(path).filename().string();
}

std::string directory(const std::string& path) {
    return fs::path(path).parent_path().string();
}

}
