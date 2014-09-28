#include "environment.h"
#include <webdriverxx/webdriver.h>
#include <gtest/gtest.h>
#include <list>

using namespace webdriverxx;

class TestJsExecutor : public ::testing::Test {
protected:
	static void SetUpTestCase() {
		Environment::Instance().GetDriver()->Navigate(
			Environment::Instance().GetTestPageUrl("js.html")
			);
	}

	TestJsExecutor() : driver(Environment::Instance().GetDriver()) {}

	WebDriver* driver;
};

TEST_F(TestJsExecutor, ExecutesSimpleScript) {
	driver->Execute("document.title = 'abc'");
	ASSERT_EQ("abc", driver->GetTitle());
}

TEST_F(TestJsExecutor, CanPassStringArgument) {
	driver->Execute("document.title = arguments[0]", JsArgs() << std::string("abc"));
	ASSERT_EQ("abc", driver->GetTitle());
}

TEST_F(TestJsExecutor, CanPassStringLiteralArgument) {
	driver->Execute("document.title = arguments[0]", JsArgs() << "abc");
	ASSERT_EQ("abc", driver->GetTitle());
}

TEST_F(TestJsExecutor, CanPassNumberArgument) {
	driver->Execute("document.title = String(arguments[0] + 21)", JsArgs() << 21);
	ASSERT_EQ("42", driver->GetTitle());
	driver->Execute("document.title = String(arguments[0] + 21)", JsArgs() << 21.5);
	ASSERT_EQ("42.5", driver->GetTitle());
}

TEST_F(TestJsExecutor, CanPassBooleanArgument) {
	driver->Execute("a = arguments; document.title = [ typeof(a[0]), a[0], typeof(a[1]), a[1] ].join(',')",
		JsArgs() << true << false);
	ASSERT_EQ("boolean,true,boolean,false", driver->GetTitle());
}

TEST_F(TestJsExecutor, CanPassMoreThanOneArgument) {
	driver->Execute("document.title = arguments[0] + ',' + arguments[1]",
		JsArgs() << "abc" << "def");
	ASSERT_EQ("abc,def", driver->GetTitle());
}

TEST_F(TestJsExecutor, CanPassElement) {
	Element e = driver->FindElement(ByTagName("input")).Clear();
	ASSERT_EQ("", e.GetAttribute("value"));
	driver->Execute("arguments[0].value = arguments[1]",
		JsArgs() << e << "abc");
	ASSERT_EQ("abc", e.GetAttribute("value"));
}

TEST_F(TestJsExecutor, CanPassArray) {
	std::vector<int> numbers;
	numbers.push_back(123);
	numbers.push_back(321);
	driver->Execute("document.title = arguments[0][0] + arguments[0][1]",
		JsArgs() << numbers);
	ASSERT_EQ("444", driver->GetTitle());
}

namespace webdriverxx {

template<typename T>
struct ToJsonImpl< std::list<T> > {
	static picojson::value Convert(const std::list<T>& value) {
		return ToJsonArray<T>(value);
	}
};

} // namespace webdriverxx

TEST_F(TestJsExecutor, CanPassCustomArray) {
	std::list<int> numbers;
	numbers.push_back(123);
	numbers.push_back(321);
	driver->Execute("document.title = arguments[0][0] + arguments[0][1]",
		JsArgs() << numbers);
	ASSERT_EQ("444", driver->GetTitle());
}

struct CustomObject {
	std::string string;
	int number;
};

namespace webdriverxx {

template<>
struct ToJsonImpl<CustomObject> {
	static picojson::value Convert(const CustomObject& value) {
		return JsonObject()
			.With("string", value.string)
			.With("number", value.number)
			.Build();
	}
};

} // namespace webdriverxx

TEST_F(TestJsExecutor, CanPassCustomObject) {
	CustomObject o = { "abc", 123 };
	driver->Execute("o = arguments[0]; document.title = (o.string + 'def') + (o.number + 1)",
		JsArgs() << o);
	ASSERT_EQ("abcdef124", driver->GetTitle());
}

///////////////////////////////////////////////////////////////////////////

TEST_F(TestJsExecutor, EvalsString) {
	ASSERT_EQ("abc", driver->Eval<std::string>("return 'abc'"));
}

TEST_F(TestJsExecutor, EvalsNumber) {
	ASSERT_EQ(123, driver->Eval<int>("return 123"));
	ASSERT_EQ(123.5, driver->Eval<double>("return 123.5"));
}

TEST_F(TestJsExecutor, EvalsBoolean) {
	ASSERT_TRUE(true == driver->Eval<bool>("return true"));
	ASSERT_TRUE(false == driver->Eval<bool>("return false"));
}

TEST_F(TestJsExecutor, EvalsElement) {
	Element e = driver->FindElement(ByTagName("input"));
	ASSERT_EQ(e, driver->EvalElement("return document.getElementsByTagName('input')[0]"));
}

namespace webdriverxx {

template<>
struct FromJsonImpl<CustomObject> {
	static CustomObject Convert(const picojson::value& value) {
		WEBDRIVERXX_CHECK(value.is<picojson::object>(), "CustomObject is not an object");
		CustomObject result;
		result.string = FromJson<std::string>(value.get("string"));
		result.number = FromJson<int>(value.get("number"));
		return result;
	}
};

} // namespace webdriverxx

TEST_F(TestJsExecutor, EvalsCustomObject) {
	CustomObject o = driver->Eval<CustomObject>("return { string: 'abc', number: 123 }");
	ASSERT_EQ("abc", o.string);
	ASSERT_EQ(123, o.number);
}

TEST_F(TestJsExecutor, EvalsArrayOfStrings) {
	std::vector<std::string> v = driver->Eval< std::vector<std::string> >(
		"return [ 'abc', 'def' ]"
		);
	ASSERT_EQ(2u, v.size());
	ASSERT_EQ("abc", v[0]);
	ASSERT_EQ("def", v[1]);
}

TEST_F(TestJsExecutor, EvalsArrayOfNumbers) {
	std::vector<int> v = driver->Eval< std::vector<int> >(
		"return [ 123, 456 ]"
		);
	ASSERT_EQ(2u, v.size());
	ASSERT_EQ(123, v[0]);
	ASSERT_EQ(456, v[1]);
}