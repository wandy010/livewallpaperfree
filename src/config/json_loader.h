#pragma once

#include <string>
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

namespace json_loader {

Json load_file(const std::string& path);
Json parse_string(const std::string& content);
bool save_file(const std::string& path, const Json& data);

std::string to_string(const Json& data, bool pretty = true);

}
