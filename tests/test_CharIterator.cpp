#include <gtest/gtest.h>

#include <fwjson/fwjson.h>

namespace fwjson {

TEST(CharIterator, TestValidity)
{
	std::stringstream stream;
	stream
		<< "line 1"
		<< "line 2"
		<< "line 3";

	CharIterator it(stream);

	while (it) {
		++it;
	}

	EXPECT_FALSE(it);
}

TEST(CharIterator, TestSkipWhitespaces)
{
	std::string input = "hello    world";
	std::istringstream stream(input);

	CharIterator it(stream);

	it.skipSpaces();
	EXPECT_EQ(it->value, 'h');

	while (it && !it->isSpace()) {
		++it;
	}

	it.skipSpaces();
	EXPECT_EQ(it->value, 'w');

	it.skipSpaces();
	EXPECT_EQ(it->value, 'w');
}

TEST(CharIterator, TestCurrentPosition)
{
	std::string input = "hello\n,\nworld";
	std::istringstream stream(input);

	CharIterator it(stream);

	while (it && !it->isSpace()) {
		++it;
	}

	EXPECT_EQ(it.currentPosition().column, 0);
	EXPECT_EQ(it.currentPosition().line, 1);

	it.skipSpaces();

	EXPECT_EQ(it.currentPosition().column, 1);
	EXPECT_EQ(it.currentPosition().line, 1);

	++it;

	EXPECT_EQ(it.currentPosition().column, 0);
	EXPECT_EQ(it.currentPosition().line, 2);

	++it;
	EXPECT_EQ(it.currentPosition().column, 1);
	EXPECT_EQ(it.currentPosition().line, 2);
}

}