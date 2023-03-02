#pragma once

#include <string>

namespace fwjson::strings {

bool toBool(const std::string&, bool* bOk);
std::string fromBool(bool value);

uint64_t toUint64(const std::string& value, bool* bOk = nullptr);
std::string fromUint64(uint64_t value);

int64_t toInt64(const std::string& value, bool* bOk = nullptr);
std::string fromInt64(int64_t value);

uint32_t toUint32(const std::string& value, bool* bOk = nullptr);
std::string fromUint32(uint32_t value);

int32_t toInt32(const std::string& value, bool* bOk = nullptr);
std::string fromInt32(int32_t value);

float toFloat(const std::string& value, bool* bOk = nullptr);
std::string fromFloat(float value);

double toDouble(const std::string& value, bool* bOk = nullptr);
std::string fromDouble(double value);

std::string_view trimLeft(const std::string& input);
std::string_view trimRight(const std::string& input);
std::string_view trim(const std::string& input);

/*
 * Convert strings like '#FF00AA', '0xFF00AA' or '0XFF00AA'
*/
uint64_t fromHexString(const std::string& in, bool* bOk = nullptr);

} // namespace fwjson::strings
