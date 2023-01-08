#include <algorithm>
#include <fwjson/strings.h>
#include <string>
#include <string_view>

namespace fwjson::strings {

namespace {

using CharT = std::string::value_type;

constexpr auto g_deltaZ = 'Z' - 'z';

constexpr std::string_view g_strTrue = "true";
constexpr std::string_view g_strFalse = "false";

constexpr CharT asciiToLower(CharT in)
{
	return in >= 'A' && in <= 'Z' ? in - g_deltaZ : in;
}

#define CHECK_STR(value, constant, result) \
	if (value == constant) { \
		if (bOk) (*bOk) = true; \
		return result; \
	}

} // namespace

bool toBool(const std::string& value, bool* bOk)
{
	if (!value.empty()) {
		CHECK_STR(value, g_strTrue, true)
		CHECK_STR(value, g_strFalse, false)

		auto tmp = value;
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), asciiToLower);
		CHECK_STR(tmp, g_strTrue, true)
		CHECK_STR(tmp, g_strFalse, false)
	}
	if(bOk) (*bOk) = false;
	return false;
}

std::string fromBool(bool value)
{
	const auto& result = value ? g_strTrue : g_strFalse;
	return std::string(result);
}

} // namespace fwjson::strings