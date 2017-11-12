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

	auto menuObject = Node::cast<Object>(object->attribute("menu"));
	ASSERT_TRUE(menuObject != nullptr);
	EXPECT_EQ(menuObject->attributesCount(), 3);
}

} // namespace fwjson
