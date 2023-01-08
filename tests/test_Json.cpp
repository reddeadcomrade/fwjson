#include "fwjson/fwjson.h"
#include <gtest/gtest.h>
#include <memory>

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

} // namespace fwjson
