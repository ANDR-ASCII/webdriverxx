#include <webdriverxx/webdriver.h>
#include <webdriverxx/browsers/chrome.h>

int main()
{
	webdriverxx::WebDriver chrome = webdriverxx::Start(webdriverxx::Chrome());
	chrome.Navigate("http://google.com")
		.FindElement(webdriverxx::ByCss("input[name=q]"))
		.SendKeys("Hello, world!")
		.Submit();
}
