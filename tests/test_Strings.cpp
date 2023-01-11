#include <memory>

#include <gtest/gtest.h>

#include <fwjson/strings.h>

namespace fwjson {

TEST(String, FromBool1)
{
	bool bOk = false;
	bool value = strings::toBool("true", &bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(value, true);
}

TEST(String, FromBool2)
{
	bool bOk = false;
	bool value = strings::toBool("True", &bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(value, true);
}

TEST(String, FromBool3)
{
	bool bOk = false;
	bool value = strings::toBool("TRUE", &bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(value, true);
}

TEST(String, FromBool4)
{
	bool bOk = false;
	bool value = strings::toBool("False", &bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(value, false);
}

TEST(String, FromBool5)
{
	bool bOk = false;
	bool value = strings::toBool("False", &bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(value, false);
}

TEST(String, FromBool6)
{
	bool bOk = false;
	bool value = strings::toBool("FALSE", &bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(value, false);
}

TEST(String, FromBool7)
{
	bool bOk = false;
	bool value = strings::toBool("value", &bOk);
	EXPECT_FALSE(bOk);
	EXPECT_EQ(value, false);
}

TEST(String, TrimLeft1)
{
	const auto result = strings::trimLeft(" hello ");
	EXPECT_EQ(result, "hello ");
}

TEST(String, TrimLeft2)
{
	const auto result = strings::trimLeft("		hello ");
	EXPECT_EQ(result, "hello ");
}

TEST(String, TrimLeft3)
{
	const auto result = strings::trimLeft(" hello world ");
	EXPECT_EQ(result, "hello world ");
}

TEST(String, TrimLeft4)
{
	const auto result = strings::trimLeft("");
	EXPECT_EQ(result, "");
}

TEST(String, TrimLeft5)
{
	const auto result = strings::trimLeft("   ");
	EXPECT_EQ(result, "");
}

TEST(String, TrimRight1)
{
	const auto result = strings::trimRight(" hello ");
	EXPECT_EQ(result, " hello");
}

TEST(String, TrimRight2)
{
	const auto result = strings::trimRight(" hello		");
	EXPECT_EQ(result, " hello");
}

TEST(String, TrimRight3)
{
	const auto result = strings::trimRight(" hello world ");
	EXPECT_EQ(result, " hello world");
}

TEST(String, TrimRight4)
{
	const auto result = strings::trimRight("");
	EXPECT_EQ(result, "");
}

TEST(String, TrimRight5)
{
	const auto result = strings::trimRight(" 	");
	EXPECT_EQ(result, "");
}

TEST(String, Trim1)
{
	const auto result = strings::trim(" hello ");
	EXPECT_EQ(result, "hello");
}

TEST(String, Trim2)
{
	const auto result = strings::trim("hello");
	EXPECT_EQ(result, "hello");
}

TEST(String, Trim3)
{
	const auto result = strings::trim("		hello world		");
	EXPECT_EQ(result, "hello world");
}

TEST(String, Trim4)
{
	const auto result = strings::trim("");
	EXPECT_EQ(result, "");
}

TEST(String, Trim5)
{
	const auto result = strings::trim("     ");
	EXPECT_EQ(result, "");
}

} // namespace fwjson