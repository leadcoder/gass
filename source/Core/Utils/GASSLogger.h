#ifndef GASS_LOGGER_H
#define GASS_LOGGER_H

#include "Core/Common.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>

// BEGIN MACROS

// Usage examples: GASS_LOG() << "raw message to log and stdout"
// or GASS_LOG(WARNING) << "auto formatted warning message to log and stderr"
// or GASS_LOG(ERROR, FILE_ONLY) << "auto formatted error message only to log file"

#define GASS_LOG GASS::Logger().Log
#define GASS_LOG_INITIALIZE GASS::Logger::Initialize
#define GASS_LOG_SET_LEVEL GASS::Logger::SetLogLevel
#define GASS_LOG_FINALIZE GASS::Logger::Finalize

// END MACROS

/**
* Enumeration of all logging levels.
* Ordered from lowest to largest logging level.
* Outside of GASS namespace only for convenience.
*/
enum LogLevel { LDEBUG = 0, LINFO, LWARNING, LERROR };

namespace GASS
{
	/**
	* Corresponding strings to logging levels enum.
	* Defined in source file
	*/
	const extern std::string LogLevelStrings[];

	/**
	* Enumeration of all output modes.
	* DISABLED: disabled all output
	* FILE_ONLY: write to log file only
	* STD_ERR: write to log file and print to stderr
	* STD_OUT: write to log file and print to stdout
	*/
	enum OutputMode { DISABLED, FILE_ONLY, STD_ERR, STD_OUT };

	class ILogListener
	{
	public:
		virtual ~ILogListener() {}
		virtual void Notify(const std::string &message) = 0;
	};

	/**
	* Basic logger class for logging errors and debugging.
	*/
	class GASSCoreExport Logger
	{
	protected:
		static bool m_Initialized; //true if Logger initialized
		static std::ofstream* m_FileStream; //log file stream object
		static bool m_AppendLogFile; //should we append or overwrite content inside log-file 
		static LogLevel m_MinLogLevel; //current minimum logging level
		static ILogListener* m_Listener;
	protected:
		LogLevel m_LogLevel; 
		OutputMode m_OutputMode; 
		std::ostringstream m_OutputStream; 
		bool m_FreeLogging; //true implies do not automatically add endl to end of string stream
	public:
		/**
		* Construct a Logger object.
		*/
		Logger();

		/**
		* Destruct a Logger object.
		*/
		~Logger();

		/**
		* Initialize the logger. Must be called before making log calls.
		*
		* You can extend this call by adding more arguments and setting new static members.
		* @param[in] logPath Path to the log file to open
		*/
		static void Initialize(const std::string &logPath);

		static bool IsInitialize();

		/**
		* Set the logging level.
		*
		* Default is INFO. Set to DEBUG to see all messages.
		* @param[in] level Minimum logging level to display messages: DEBUG|INFO|WARNING|ERROR
		*/
		static void SetLogLevel(LogLevel level);

		/**
		* Finalize the logger. Must be called before exiting application to release file handler.
		*/
		static void Finalize();


		/**
		* Set optional log listener.
		* @param[in] listener Pointer to user defined log-listener class derived from ILogListener 
		*/
		static void SetListener(ILogListener* listener);

		/**
		* Write to the log raw stream.
		* @return String stream object for internal use
		*/
		std::ostringstream& Log();

		/**
		* Write to the log using the specified logging level.
		* @param[in] level Logging level of message: DEBUG|INFO|WARNING|ERROR
		* @return String stream object for internal use
		*/
		std::ostringstream& Log(LogLevel level);

		/**
		* Write to the log using the specified logging level
		* and writing mode.
		* @param[in] level Logging level of message: DEBUG|INFO|WARNING|ERROR
		* @param[in] mode Output mode: FILE_ONLY|STD_ERR|STD_OUT
		* @return String stream object for internal use
		*/
		std::ostringstream& Log(LogLevel level, OutputMode mode);
	};

	/**
	* Basic date and time helper class.
	*/
	class GASSCoreExport DateTimeHelper
	{
	public:
		/**
		* Get the time elapsed since program started (in milliseconds).
		* @return Milliseconds elapsed since program started
		*/
		static double getTimeElapsed();

		/**
		* Get the current date and time as a string.
		* @return Current date and time string
		*/
		static std::string getDateTime();

		/**
		* Get the current time as a string.
		* @return Current time string
		*/
		static std::string getTime();
	};
}

#endif