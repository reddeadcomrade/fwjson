#include <gtest/gtest.h>

#include <fwjson/parsing.h>

namespace fwjson {

TEST(ParsingHexStrings, ParsingValidInput1)
{
	bool error = false;
	const auto result = parsing::parseHexString("#FF00AA", &error);
	EXPECT_FALSE(error);
	EXPECT_EQ(result, 0xFF00AA);
}

TEST(ParsingHexStrings, ParsingValidInput2)
{
	bool error = false;
	const auto result = parsing::parseHexString("#ff00aa", &error);
	EXPECT_FALSE(error);
	EXPECT_EQ(result, 0xFF00AA);
}

TEST(ParsingHexStrings, ParsingValidInput3)
{
	bool error = false;
	const auto result = parsing::parseHexString("#ff00aacc", &error);
	EXPECT_FALSE(error);
	EXPECT_EQ(result, 0xff00aacc);
}

TEST(ParsingHexStrings, ParsingValidInput4)
{
	bool error = false;
	const auto result = parsing::parseHexString("0xff00aa", &error);
	EXPECT_FALSE(error);
	EXPECT_EQ(result, 0xff00aa);
}

TEST(ParsingHexStrings, ParsingValidInput5)
{
	bool error = false;
	const auto result = parsing::parseHexString("0xFF00AA", &error);
	EXPECT_FALSE(error);
	EXPECT_EQ(result, 0xff00aa);
}

TEST(ParsingHexStrings, ParsingValidInput6)
{
	bool error = false;
	const auto result = parsing::parseHexString("0XFF00AA", &error);
	EXPECT_FALSE(error);
	EXPECT_EQ(result, 0xff00aa);
}

TEST(ParsingHexStrings, ParsingValidInput7)
{
	bool error = false;
	const auto result = parsing::parseHexString("123456", &error);
	EXPECT_FALSE(error);
	EXPECT_EQ(result, 0x123456);
}

TEST(ParsingHexStrings, ParsingBadInput1)
{
	bool error = false;
	parsing::parseHexString("", &error);
	EXPECT_TRUE(error);
}

TEST(ParsingHexStrings, ParsingBadInput2)
{
	bool error = false;
	parsing::parseHexString("#", &error);
	EXPECT_TRUE(error);
}

TEST(ParsingHexStrings, ParsingBadInput3)
{
	bool error = false;
	parsing::parseHexString("0x", &error);
	EXPECT_TRUE(error);
}

TEST(ParsingHexStrings, ParsingBadInput4)
{
	bool error = false;
	parsing::parseHexString("XYZ", &error);
	EXPECT_TRUE(error);
}

TEST(ParsingHexStrings, ParsingBadInput5)
{
	bool error = false;
	parsing::parseHexString("#XYZ", &error);
	EXPECT_TRUE(error);
}

TEST(ParsingHexStrings, ParsingBadInput6)
{
	bool error = false;
	parsing::parseHexString("0xXYZ", &error);
	EXPECT_TRUE(error);
}

} // namespace gs