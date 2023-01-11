#pragma once

#include <string>

namespace fwjson::parsing {

/*
 * Parse strings like '#FF00AA', '0xFF00AA' or 'FF00AA'
*/
uint64_t parseHexString(const std::string& input, bool* error = nullptr);

} // namespace fwjson::parsing