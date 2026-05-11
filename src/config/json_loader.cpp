#include "json_loader.h"
#include "utils/file_utils.h"

#include <fstream>

namespace json_loader {

Json load_file(const std::string& path) {
    auto content = file_utils::read_file(path);
    if (content.empty()) {
        return Json::object();
    }
    return parse_string(content);
}

Json parse_string(const std::string& content) {
    try {
        return Json::parse(content);
    } catch (const Json::parse_error& e) {
        return Json::object();
    }
}

bool save_file(const std::string& path, const Json& data) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    file << data.dump(4);
    return true;
}

std::string to_string(const Json& data, bool pretty) {
    return pretty ? data.dump(4) : data.dump();
}

}
