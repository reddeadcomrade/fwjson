#pragma once

#include <string>

namespace fwjson::strings {

bool toBool(const std::string&, bool* bOk);
std::string fromBool(bool value);

std::string_view trimLeft(const std::string& input);
std::string_view trimRight(const std::string& input);
std::string_view trim(const std::string& input);

} // namespace fwjson::strings
