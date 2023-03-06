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

TEST(Json, Parse2)
{
    std::string json = "{\
        \"background\" : \"123.png\",\
        \"size\" : {\
            \"width\" : 100,\
            \"height\" : 100\
        },\
        \"number array\" : [-56, 1, 2.25, 3e-3, 5.],\
        \"stroka\"       : \"value_str\",\
        \"stroka 2\"     : \"\u041f\u0440\u0438\u0432\u0435\u0442 \u043c\u0438\u0440!\",\
        \"zero_str\"     : \"\",\
        \"visible\"      : true,\
        \"visible_str\" : \"true\"\
    }";

    auto object = std::make_unique<Object>();
    object->parse(json);
    EXPECT_EQ(object->attributesCount(), 8);

    auto background = object->child<String>("background");
    EXPECT_TRUE(background);
    EXPECT_EQ(background->value(), "123.png");

    auto size = object->child<Object>("size");
    EXPECT_TRUE(size);
    EXPECT_EQ(size->attributesCount(), 2);

    auto str = object->child<String>("stroka 2");
    EXPECT_TRUE(str);
    EXPECT_EQ(str->value(), "Привет мир!");
}

TEST(Json, SingleLineComment)
{
	std::string json =
	"// json file describes menu \n"

	"{\"menu\": {\n"
		"// this is \"menu\" section \n"
		"\"id\": \"file\", // file submenu \n"
		"\"value\": \"File\", // value submenu \n"

		"// this is popup section \n"
		"\"popup\": { \n"
	       "// popup menu items \n"
			"\"menuitem\": [\n"
	           "// items to work with doc \n"
				"{\n"
					"// this item creates new doc \n"
					"\"value\": \"New\",             // item title \n"
					"\"onclick\": \"CreateNewDoc()\" // item callback \n"
				"},\n"

				"// other items \n"
				"{\"value\": \"Open\", \"onclick\": \"OpenDoc()\"},\n"
				"{\"value\": \"Close\", \"onclick\": \"CloseDoc()\"}\n"
			"]\n"
		"}\n"
	"}}\n"
	"// end of json file";

	auto object = std::make_unique<Object>();
	object->parse(json);

	EXPECT_EQ(object->attributesCount(), 1);

	auto menuObject = object->child<Object>("menu");
	ASSERT_TRUE(menuObject != nullptr);
	EXPECT_EQ(menuObject->attributesCount(), 3);
}

TEST(Json, MultiLineComment)
{
	std::string json =
		"/* \n"
		" * This json file describes menu \n"
		" * menu contains a few submenu items \n"
		"*/ \n"

		"{\"menu\": {\n"
			"/* this is \"menu\" section */ \n"
			"\"id\": \"file\",    /* file submenu  */ \n"
			"\"value\": \"File\", /* value submenu */ \n"

			"/* \n"
			" * this is popup section \n"
			" */ \n"

			"\"popup\": { \n"
				"/* popup menu items */ \n"
				"\"menuitem\": [\n"
					"/* items to work with doc */ \n"
					"{\n"
						"/* this item creates new doc     \n"
						"    - value is a menu title      \n"
						"    - onclick is a menu callback \n"
						" */ \n"

						"\"value\":   \"New\",           /* item title    */ \n"
						"\"onclick\": \"CreateNewDoc()\" /* item callback */ \n"
					"},\n"

					"/* Open documents */ \n"
					"{\n"
						"\"value\":   \"Open\", \n"
						"\"onclick\": \"OpenDoc()\" \n"
					"},\n"

					"/* Close documents */ \n"
					"{\n"
						"\"value\":   /* Menu title    */ \"Close\", \n"
						"\"onclick\": /* Menu callback */ \"CloseDoc()\" \n"
					"}\n"
				"]\n"
			"}\n"
		"}}\n"
		"/* end of json file */ \n";

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
