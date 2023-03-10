#include <fwjson/char_iterator.h>

namespace fwjson {

CharIterator::CharIterator(std::istream& stream)
	: m_stream(stream)
	, m_current(0)
{
	next();
}

CharIterator::operator bool() const
{
	return isValid();
}

CharIterator& CharIterator::operator++()
{
	next();
	return *this;
}

const Symbol* CharIterator::operator->() const
{
	return &m_current;
}

bool CharIterator::operator==(char c) const
{
	return m_current.value == c;
}

Symbol* CharIterator::operator->()
{
	return &m_current;
}

void CharIterator::skipSpaces()
{
	while (std::isspace(m_current.value) && next()) {}
}

const TextPosition& CharIterator::currentPosition() const
{
	return m_position;
}

std::size_t CharIterator::nextToken(const std::span<std::string>& tokens, const CharIterator::SuffixOpt& callback)
{
	if (tokens.empty()) {
		return 0;
	}

	const auto originPosition = m_stream.tellg();
	const auto originValue = m_current;

	if (isValid()) {
		std::string buffer;
		if (callback) {
			while (isValid() && (*callback)(m_current)) {
				buffer += m_current.value;
				next();
			}
		} else {
			m_stream.seekg(originPosition - std::streamoff{1});
			m_stream >> buffer;
			next();
		}

		const auto it = std::find(tokens.begin(), tokens.end(), buffer);
		if (it != tokens.end()) {
			return std::distance(tokens.begin(), it);
		}
	}

	m_stream.seekg(originPosition);
	m_current = originValue;
	return tokens.size();
}

bool CharIterator::next()
{
	if (isValid()) {
		const char sym = m_stream.get();
		if (m_stream.gcount() > 0) {
			m_current = sym;
			m_position.add(sym);
			return true;
		}
	}

	m_current = 0;
	return false;
}

bool CharIterator::isValid() const
{
	return !m_stream.eof() && !m_stream.fail();
}

} // namespace fwjson