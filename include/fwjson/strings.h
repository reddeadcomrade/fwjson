#pragma once

#include <string>

namespace fwjson::strings {

bool toBool(const std::string&, bool* bOk);
std::string fromBool(bool value);

} // namespace fwjson::strings
