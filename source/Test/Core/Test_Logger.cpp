#include "Core/Utils/GASSLogger.h"
#include "catch.hpp"


class TestListener : public GASS::ILogListener
{
public:
	void Notify(const std::string &message)
	{
		m_Message = message;
	}
	std::string m_Message;
};

TEST_CASE("Test Logger")
{
	SECTION("Test Listener")
	{
		std::string message = "Test LINFO";
		TestListener listener;
		GASS::Logger::SetListener(&listener);
		GASS::Logger::SetLogLevel(LINFO);
		GASS_LOG(LINFO) << message;
		//Check that we got message in listener
		REQUIRE(listener.m_Message.find(message) != std::string::npos);

		//Change log level and check that we don't get any message 
		GASS::Logger::SetLogLevel(LWARNING);
		listener.m_Message = "";
		GASS_LOG(LINFO) << message;
		REQUIRE(listener.m_Message == "");

		listener.m_Message = "";
		GASS_LOG(LDEBUG) << message;
		REQUIRE(listener.m_Message == "");

		listener.m_Message = "";
		GASS_LOG(LDEBUG) << message;
		REQUIRE(listener.m_Message == "");


		//remove listener
		GASS::Logger::SetListener(NULL);
	}
}