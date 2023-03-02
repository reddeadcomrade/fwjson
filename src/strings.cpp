#include <algorithm>
#include <string>
#include <string_view>
#include <numeric>

#include <fwjson/strings.h>

namespace fwjson::strings {

namespace {

using CharT = std::string::value_type;

constexpr auto g_deltaZ = 'Z' - 'z';

constexpr std::string_view g_strTrue = "true";
constexpr std::string_view g_strFalse = "false";

constexpr std::string_view g_inf= "Infinity";
constexpr std::string_view g_nInf = "-Infinity";

constexpr CharT asciiToLower(CharT in)
{
	return in >= 'A' && in <= 'Z' ? in - g_deltaZ : in;
}

bool isEquals(const std::string_view& a, const std::string_view& b)
{
    return std::equal(a.begin(), a.end(), b.begin(), b.end(),
        [](char a, char b) {
            return tolower(a) == tolower(b);
        }
    );
}

#define CHECK_STR(value, constant, result) \
	if (value == constant) { \
		if (bOk) (*bOk) = true; \
		return result; \
	}

template<class T> requires std::is_integral<T>::value
struct IntConverter
{
    using ConverterPtr = T(const std::string&, std::size_t*, int);

    static T convert(const std::string& value, bool* bOk, ConverterPtr converter)
    {
        if (const auto str = trim(value); !str.empty()) {
            if constexpr (std::is_same<T, uint64_t>::value || std::is_same<T, uint32_t>::value) {
                bool bOkHex = false;
                T fromHexResult = fromHexString(str.data(), &bOkHex);
                if (bOkHex) {
                    if (bOk) *bOk = true;
                    return fromHexResult;
                }
            }

            try {
                std::size_t pos = 0;
                T result = converter(str.data(), &pos, 10);
                if (bOk) *bOk = pos == str.size();
                return result;
            }  catch (...) {}
        }

        if (bOk) *bOk = false;
        return 0;
    }
};

template<class T> requires std::is_floating_point<T>::value
struct RealConverter
{
    using ConverterPtr = T(const std::string&, std::size_t*);

    static T convert(const std::string& value, bool* bOk, ConverterPtr converter)
    {
        if (const auto str = trim(value); !str.empty()) {
            if (isEquals(str, g_inf)) {
                if (bOk) *bOk = true;
                return std::numeric_limits<T>::infinity();
            }
            if (isEquals(str, g_nInf)) {
                if (bOk) *bOk = true;
                return -std::numeric_limits<T>::infinity();
            }

            try {
                std::size_t pos = 0;
                T result = converter(str.data(), &pos);
                if (bOk) *bOk = pos == str.size();
                return result;
            }  catch (...) {}
        }

        if (bOk) *bOk = false;
        return 0;
    }
};

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

std::string_view trimLeft(const std::string& input)
{
	auto it = std::find_if(input.cbegin(), input.cend(), [](auto ch){
		return !std::isspace(ch);
	});
	return { it, input.end() };
}

std::string_view trimRight(const std::string& input)
{
	auto it = std::find_if(input.crbegin(), input.crend(), [](auto ch){
		return !std::isspace(ch);
	});
	return { input.begin(), it.base() };
}

std::string_view trim(const std::string& input)
{
	std::string_view left = trimLeft(input);
	std::string_view right = trimRight(input);
	if (left.begin() == left.end() || right.begin() == right.end()) {
		return std::string_view();
	}
	return { left.begin(), right.end() };
}

uint64_t toUint64(const std::string& value, bool* bOk)
{
	return IntConverter<uint64_t>::convert(value, bOk, std::stoul);
}

std::string fromUint64(uint64_t value)
{
	return std::to_string(value);
}

int64_t toInt64(const std::string& value, bool* bOk)
{
	return IntConverter<int64_t>::convert(value, bOk, std::stol);
}

std::string fromInt64(int64_t value)
{
	return std::to_string(value);
}

uint32_t toUint32(const std::string& value, bool* bOk)
{
	return IntConverter<uint64_t>::convert(value, bOk, std::stoul);
}

std::string fromUint32(uint32_t value)
{
	return std::to_string(value);
}

int32_t toInt32(const std::string& value, bool* bOk)
{
	return IntConverter<int32_t>::convert(value, bOk, std::stoi);
}

std::string fromInt32(int32_t value)
{
	return std::to_string(value);
}

float toFloat(const std::string& value, bool* bOk)
{
	return RealConverter<float>::convert(value, bOk, std::stof);
}

std::string fromFloat(float value)
{
	const auto size = std::snprintf(nullptr, 0, "%.8g", value);
	std::string result(size, 0);
	std::sprintf(result.data(), "%.8g", value);
	return { result.begin(), result.end() };
}

double toDouble(const std::string& value, bool* bOk)
{
    return RealConverter<double>::convert(value, bOk, std::stod);
}

std::string fromDouble(double value)
{
    const auto size = std::snprintf(nullptr, 0, "%.16g", value);
    std::string result(size, 0);
    std::sprintf(result.data(), "%.16g", value);
    return { result.begin(), result.end() };
}

uint64_t fromHexString(const std::string& input, bool* bOk)
{
	if (const auto str = trim(input); !str.empty()) {
		uint32_t offset = 0;
		if (str.starts_with('#')) {
			offset = 1;
		} else if (str.starts_with("0x") || str.starts_with("0X")) {
			offset = 2;
		}

		if (offset && offset < str.size()) {
			std::size_t pos = 0;
			try {
				uint64_t result = std::stoull(str.data() + offset, &pos, 16);
				if (bOk) *bOk = pos == str.size() - offset;
				return result;
			}  catch (...) {}
		}
	}

	if (bOk) *bOk = false;
	return 0;
}

} // namespace fwjson::strings