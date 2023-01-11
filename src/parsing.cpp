#include <fwjson/parsing.h>

namespace fwjson::parsing {

uint64_t parseHexString(const std::string& input, bool* error)
{
	uint32_t offset = 0;
	if (input.starts_with('#')) {
		offset = 1;
	} else if (input.starts_with("0x") || input.starts_with("0X")) {
		offset = 2;
	}

	if (input.size() > offset) {
		try {
			uint64_t result = std::stoull(input.data() + offset, nullptr, 16);
			if (error) *error = false;
			return result;
		}  catch (...) {}
	}

	if (error) *error = true;
	return 0;
}

} // namespace fwjson::parsing