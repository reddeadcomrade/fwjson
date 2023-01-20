#include <gtest/gtest.h>

#include <fwjson/strings.h>

namespace fwjson {

TEST(ParsingHexStrings, ParsingValidInput1)
{
	bool bOk = false;
	const auto result = strings::fromHexString("#FF00AA", &bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(result, 0xFF00AA);
}

TEST(ParsingHexStrings, ParsingValidInput2)
{
	bool bOk = false;
	const auto result = strings::fromHexString("#ff00aa", &bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(result, 0xFF00AA);
}

TEST(ParsingHexStrings, ParsingValidInput3)
{
	bool bOk = false;
	const auto result = strings::fromHexString("#ff00aacc", &bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(result, 0xff00aacc);
}

TEST(ParsingHexStrings, ParsingValidInput4)
{
	bool bOk = false;
	const auto result = strings::fromHexString("0xff00aa", &bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(result, 0xff00aa);
}

TEST(ParsingHexStrings, ParsingValidInput5)
{
	bool bOk = false;
	const auto result = strings::fromHexString("0xFF00AA", &bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(result, 0xff00aa);
}

TEST(ParsingHexStrings, ParsingValidInput6)
{
	bool bOk = false;
	const auto result = strings::fromHexString("0XFF00AA", &bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(result, 0xff00aa);
}

TEST(ParsingHexStrings, ParsingBadInput1)
{
	bool bOk = false;
	strings::fromHexString("", &bOk);
	EXPECT_FALSE(bOk);
}

TEST(ParsingHexStrings, ParsingBadInput2)
{
	bool bOk = false;
	strings::fromHexString("#", &bOk);
	EXPECT_FALSE(bOk);
}

TEST(ParsingHexStrings, ParsingBadInput3)
{
	bool bOk = false;
	strings::fromHexString("0x", &bOk);
	EXPECT_FALSE(bOk);
}

TEST(ParsingHexStrings, ParsingBadInput4)
{
	bool bOk = false;
	strings::fromHexString("XYZ", &bOk);
	EXPECT_FALSE(bOk);
}

TEST(ParsingHexStrings, ParsingBadInput5)
{
	bool bOk = false;
	strings::fromHexString("#XYZ", &bOk);
	EXPECT_FALSE(bOk);
}

TEST(ParsingHexStrings, ParsingBadInput6)
{
	bool bOk = false;
	strings::fromHexString("0xXYZ", &bOk);
	EXPECT_FALSE(bOk);
}

TEST(ParsingHexStrings, ParsingBadInput7)
{
	bool bOk = false;
	strings::fromHexString("123456", &bOk);
	EXPECT_FALSE(bOk);
}

} // namespace gs