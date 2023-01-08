#include <memory>

#include <gtest/gtest.h>

#include <fwjson/fwjson.h>

namespace fwjson {

TEST(Json, Parse)
{
	std::string json = "{\"menu\": {\
		\"id\": \"file\",\
		\"value\": \"File\",\
		\"popup\": {\
			\"menuitem\": [\
				{\"value\": \"New\", \"onclick\": \"CreateNewDoc()\"},\
				{\"value\": \"Open\", \"onclick\": \"OpenDoc()\"},\
				{\"value\": \"Close\", \"onclick\": \"CloseDoc()\"}\
			]\
		}\
		}}";

	auto object = std::make_unique<Object>();
	object->parse(json);

	EXPECT_EQ(object->attributesCount(), 1);

	auto menuObject = object->child<Object>("menu");
	ASSERT_TRUE(menuObject != nullptr);
	EXPECT_EQ(menuObject->attributesCount(), 3);
}

TEST(Json, Double)
{
	std::string json = "{"
		"\"number array\" : [-56, 1, 2.25, 5e-3, 5.]"
		"}";

	auto object = std::make_unique<Object>();
	object->parse(json);

	auto* numbers = object->child<Array>("number array");
	ASSERT_TRUE(numbers != nullptr);

	EXPECT_EQ(numbers->size(), 5);
	ASSERT_TRUE(numbers->child<Number>(0) != nullptr);
	ASSERT_TRUE(numbers->child<Number>(1) != nullptr);
	ASSERT_TRUE(numbers->child<Number>(2) != nullptr);
	ASSERT_TRUE(numbers->child<Number>(3) != nullptr);
	ASSERT_TRUE(numbers->child<Number>(4) != nullptr);

	EXPECT_EQ(numbers->child<Number>(0)->value(), -56);
	EXPECT_EQ(numbers->child<Number>(1)->value(), 1);
	EXPECT_EQ(numbers->child<Number>(2)->value(), 2.25);
	EXPECT_EQ(numbers->child<Number>(3)->value(), 0.005);
	EXPECT_EQ(numbers->child<Number>(4)->value(), 5);
}

TEST(Json, StringToBool1)
{
	bool bOk = false;
	auto str = std::make_unique<String>("true");

	auto value = str->toBool(&bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(value, true);
}

TEST(Json, StringToBool2)
{
	bool bOk = false;
	auto str = std::make_unique<String>("True");

	auto value = str->toBool(&bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(value, true);
}

TEST(Json, StringToBool3)
{
	bool bOk = false;
	auto str = std::make_unique<String>("TRUE");

	auto value = str->toBool(&bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(value, true);
}

TEST(Json, StringToBool4)
{
	bool bOk = false;
	auto str = std::make_unique<String>("false");

	auto value = str->toBool(&bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(value, false);
}

TEST(Json, StringToBool5)
{
	bool bOk = false;
	auto str = std::make_unique<String>("False");

	auto value = str->toBool(&bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(value, false);
}

TEST(Json, StringToBool6)
{
	bool bOk = false;
	auto str = std::make_unique<String>("FALSE");

	auto value = str->toBool(&bOk);
	EXPECT_TRUE(bOk);
	EXPECT_EQ(value, false);
}

TEST(Json, StringToBool7)
{
	bool bOk = false;
	auto str = std::make_unique<String>("value");

	auto value = str->toBool(&bOk);
	EXPECT_FALSE(bOk);
	EXPECT_EQ(value, false);
}

TEST(Json, StringToBool8)
{
	bool bOk = false;
	auto str = std::make_unique<String>();

	auto value = str->toBool(&bOk);
	EXPECT_FALSE(bOk);
	EXPECT_EQ(value, false);
}

} // namespace fwjson
