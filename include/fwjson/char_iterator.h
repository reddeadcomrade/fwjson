#include <string>
#include <istream>
#include <span>
#include <optional>
#include <functional>

namespace fwjson {

struct TextPosition {
	void add(char sym) {
		if (sym == '\n') {
			column = 0;
			++line;
		} else {
			++column;
		}
	}


	uint32_t line = 0;
	uint32_t column = 0;
};

struct Symbol {
	Symbol(char c) : uValue(c), value(c) {}
	Symbol& operator=(char c) {
		uValue = c;
		value = c;
		return *this;
	}

	inline bool isSpace() const {
		return std::isspace(value);
	}

	uint8_t uValue = 0;
	char value = 0;
};

class CharIterator {
public:
	explicit CharIterator(std::istream& stream);
	~CharIterator() = default;

	using Suffix = std::function<bool(const Symbol&)>;
	using SuffixOpt = std::optional<Suffix>;

public:
	explicit operator bool() const;
	const Symbol* operator->() const;
	bool operator==(char c) const;

	CharIterator& operator++();
	Symbol* operator->();

	const TextPosition& currentPosition() const;

	void skipSpaces();
	std::size_t nextToken(const std::span<std::string>& tokens, const SuffixOpt& callback = std::nullopt);

private:
	bool next();
	bool isValid() const;

private:
	std::istream& m_stream;
	Symbol m_current;
	TextPosition m_position;
};

} // namespace fwjson