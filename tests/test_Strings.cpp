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

} // namespace fwjson