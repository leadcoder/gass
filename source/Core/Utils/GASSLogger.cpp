#include "Core/Utils/GASSLogger.h"
#include <iomanip>

namespace GASS
{
	// define log level strings here
	const std::string LogLevelStrings[] = { "DEBUG", "INFO", "WARNING", "ERROR" };
	
	bool Logger::m_AppendLogFile = false;
	bool Logger::m_Initialized = false;
	std::ofstream* Logger::m_FileStream = nullptr;
	ILogListener* Logger::m_Listener = nullptr;
	LogLevel Logger::m_MinLogLevel = LINFO;

	Logger::Logger() : m_FreeLogging(false),
		m_LogLevel(LINFO),
		m_OutputMode(STD_OUT)
	{
		
	}

	Logger::~Logger()
	{
		// only write to log/output if logging level is at least at minimum logging level
		if (m_LogLevel >= Logger::m_MinLogLevel)
		{
			if (!m_FreeLogging)
				m_OutputStream << std::endl;

			// write to log file
			if (Logger::m_Initialized)
			{
				(*m_FileStream) << m_OutputStream.str();
				(*m_FileStream).flush();
			}

			// also write to output
			if (m_OutputMode == STD_ERR)
			{
				std::cerr << m_OutputStream.str();
				std::cerr.flush();
			}
			else if (m_OutputMode == STD_OUT)
			{
				std::cout << m_OutputStream.str();
				std::cout.flush();
			}

			if (m_Listener)
				m_Listener->Notify(m_OutputStream.str());
		}
	}

	void Logger::Initialize(const std::string &logPath)
	{
		// You can change the default minimum logging level here
		m_MinLogLevel = LINFO;

		// create new file log stream object
		m_FileStream = new std::ofstream(logPath.c_str(), m_AppendLogFile ? std::ios_base::app : std::ios_base::out);

		// You can customize the initial logging output below
		(*m_FileStream) << "========== START: " << DateTimeHelper::getDateTime() << " ==========" << std::endl << std::endl;

		// initialized and ready to write to log file
		m_Initialized = true;
	}

	bool Logger::IsInitialize()
	{
		return m_Initialized;
	}

	void Logger::SetLogLevel(LogLevel level)
	{
		m_MinLogLevel = level;
	}

	void Logger::SetListener(ILogListener* listener)
	{
		m_Listener = listener;
	}

	void Logger::Finalize()
	{
		m_Initialized = false;
		if (m_FileStream != nullptr)
		{
			// You can customize the final logging output below
			(*m_FileStream) << std::endl << "========== END: " << DateTimeHelper::getDateTime() << " ==========" << std::endl << std::endl;

			// close file log stream
			m_FileStream->close();

			// delete it
			delete m_FileStream;
		}
	}

	std::ostringstream& Logger::Log()
	{
		// You can change the default logging behavior here
		m_LogLevel = LINFO;
		m_OutputMode = STD_OUT;

		return m_OutputStream;
	}

	std::ostringstream& Logger::Log(LogLevel level)
	{
		// You can change here whether to print to stdout or stderr 
		// depending on the logging level passed
		if (level >= LWARNING)
			return Log(level, STD_ERR);
		else
			return Log(level, STD_OUT);
	}

	std::ostringstream& Logger::Log(LogLevel level, OutputMode mode)
	{
		m_FreeLogging = false;

		/*if (!m_Initialized)
		{
			Log() << "[" << DateTimeHelper::getTime() << "] " << "(" << LogLevelStrings[level] << ") ";
			return Log();
		}*/

		m_LogLevel = level;
		m_OutputMode = mode;

		// You can customize below to control logging output formatting
		// The default is "[time_elapsed] (log_level) message"
		m_OutputStream << "[" << DateTimeHelper::getTime() << "] "
			<< "(" << LogLevelStrings[level] << ") ";
		return m_OutputStream;
	}

	double DateTimeHelper::getTimeElapsed()
	{
		return (double)clock() / CLOCKS_PER_SEC;
	}

	std::string DateTimeHelper::getDateTime()
	{
		time_t t = time(nullptr);   // get time now
		struct tm * now = localtime(&t);
		std::stringstream ss;
		ss << (now->tm_year + 1900) << '-'
			<< std::setw(2) << std::setfill('0') << (now->tm_mon + 1) << '-'
			<< std::setw(2) << std::setfill('0') << now->tm_mday << " "
			<< std::setw(2) << std::setfill('0') << now->tm_hour << ":"
			<< std::setw(2) << std::setfill('0') << now->tm_min << ":"
			<< std::setw(2) << std::setfill('0') << now->tm_sec;
		return ss.str();
	}

	std::string DateTimeHelper::getTime()
	{
		time_t t = time(nullptr);   // get time now
		struct tm * now = localtime(&t);
		std::stringstream ss;
		ss  << std::setw(2) << std::setfill('0') << now->tm_hour << ":"
			<< std::setw(2) << std::setfill('0') << now->tm_min << ":"
			<< std::setw(2) << std::setfill('0') << now->tm_sec;
		return ss.str();
	}
}