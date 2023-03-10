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

TEST(CharIterator, TestNextToken)
{
	{
		std::string input = "hello world";
		std::istringstream stream(input);

		CharIterator it(stream);
		EXPECT_EQ(it->value, 'h');

		{
			std::array<std::string, 2> names = {
				"hello",
				"world"
			};
			const auto r1 = it.nextToken(names);
			EXPECT_EQ(r1, 0);
			EXPECT_EQ(it->value, ' ');
		}

		++it;
		EXPECT_EQ(it->value, 'w');

		{
			std::vector<std::string> v = {
				"world"
			};
			const auto r2 = it.nextToken(v);
			EXPECT_EQ(r2, 0);
			EXPECT_EQ(it->value, 0);
		}
	}
	{
		std::string input = "hello1 world";
		std::istringstream stream(input);

		CharIterator it(stream);

		std::array<std::string, 3> names = {
				"hello",
				"hello1",
				"world"
		};
		auto result = it.nextToken(names, [](auto& symbol){
			return !symbol.isSpace();
		});
		EXPECT_EQ(result, 1);
		EXPECT_EQ(it->value, ' ');
	}
	{
		std::string input = "lorem ipsum";
		std::istringstream stream(input);

		CharIterator it(stream);

		std::array<std::string, 2> names = {
			"hello",
			"world"
		};
		auto result = it.nextToken(names, [](auto& symbol){
			return symbol.isSpace();
		});
		EXPECT_EQ(result, names.size());
		EXPECT_EQ(it->value, 'l');
	}
	{
		std::string input = "hello1 world1";
		std::istringstream stream(input);

		CharIterator it(stream);

		std::array<std::string, 2> names = {
			"hello",
			"world"
		};
		auto result = it.nextToken(names, [](auto& symbol){
			return symbol.isSpace();
		});
		EXPECT_EQ(result, names.size());
		EXPECT_EQ(it->value, 'h');
	}
	{
		std::string input = "hell o worl d";
		std::istringstream stream(input);

		CharIterator it(stream);

		std::array<std::string, 2> names = {
			"hello",
			"world"
		};
		auto result = it.nextToken(names, [](auto& symbol){
			return symbol.isSpace();
		});
		EXPECT_EQ(result, names.size());
		EXPECT_EQ(it->value, 'h');
	}
	{
		std::string input = "hello";
		std::istringstream stream(input);

		CharIterator it(stream);

		std::array<std::string, 2> names = {
			"hello",
			"world"
		};
		auto result = it.nextToken(names, [](auto& symbol){
			return !symbol.isSpace();
		});
		EXPECT_EQ(result, 0);
		EXPECT_EQ(it->value, 0);
	}
	{
		std::string input = "lorem";
		std::istringstream stream(input);

		CharIterator it(stream);

		std::array<std::string, 2> names = {
				"hello",
				"world"
		};
		auto result = it.nextToken(names, [](auto& symbol){
			return !symbol.isSpace();
		});
		EXPECT_EQ(result, names.size());
		EXPECT_EQ(it->value, 'l');
	}
	{
		std::string input = "hello1 world";
		std::istringstream stream(input);

		CharIterator it(stream);

		std::array<std::string, 2> names = {
			"hello",
			"world"
		};
		auto result = it.nextToken(names, [](auto& symbol){
			return symbol.value != '1';
		});
		EXPECT_EQ(result, 0);
		EXPECT_EQ(it->value, '1');
	}
	{
		std::string input = "lorem1 ipsum";
		std::istringstream stream(input);

		CharIterator it(stream);

		std::array<std::string, 4> names = {
			"one",
			"two",
			"three",
			"hello1"
		};
		auto result = it.nextToken(names, [](auto& symbol){
			return symbol.value != '1';
		});
		EXPECT_EQ(result, names.size());
		EXPECT_EQ(it->value, 'l');
	}
}

} // namespace fwjson