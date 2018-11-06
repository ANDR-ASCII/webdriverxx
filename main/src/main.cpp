#include <webdriverxx/webdriver.h>
#include <webdriverxx/browsers/chrome.h>

int main()
{
	try
	{
		webdriverxx::WebDriver chrome = webdriverxx::Start(webdriverxx::Chrome());

		const webdriverxx::Session& session = chrome.Navigate("https://rivesolutions.com");
		std::cout << session.GetTitle() << std::endl;
	}
	catch (const std::exception& exception)
	{
		std::cout << exception.what();
	}

	std::cin.get();
}
