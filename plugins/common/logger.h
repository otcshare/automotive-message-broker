/*
Copyright (C) 2012 Intel Corporation

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <ostream>
#include <string>
#include <sstream>
#include <sys/time.h>
#include <sys/types.h>

// Comment this line if you don't need multithread support
#define LOGGER_MULTITHREAD
#ifdef LOGGER_MULTITHREAD
#include <pthread.h>
#endif

/** \addtogroup libamb-plugins-common
 *  @{
 */

namespace CUtil {

/**
 * \brief Simple logger to log messages on file and console.
 *
 * This is the implementation of a simple logger in C++. It is implemented
 * as a Singleton, so it can be easily called through two DEBUG macros.
 * It is Pthread-safe.
 * It allows to log on both file and screen, and to specify a verbosity
 * threshold for both of them.
 *
 * @class Logger
 */
class Logger
{
	/**
	 * Type used for the configuration
	 */
	enum loggerConf_	{L_nofile_	= 	1 << 0,
				L_file_		=	1 << 1,
				L_noscreen_	=	1 << 2,
				L_screen_	=	1 << 3};

#ifdef LOGGER_MULTITHREAD
	/**
	 * \brief Lock for mutual exclusion between different threads
	 */
	static pthread_mutex_t lock_;
#endif

	bool configured_;

	/**
	 * \brief Pointer to the unique Logger (i.e., Singleton)
	 */
	static Logger* m_;

	/**
	 * \brief Initial part of the name of the file used for Logging.
	 * Date and time are automatically appended.
	 */
	std::string logFile_;

	/**
	 * \brief Initial part of the name of the file used for Logging.
	 * Date and time are automatically appended.
	 */
	std::string logFileName_;

	/**
	 * \brief Current configuration of the logger.
	 * Variable to know if logging on file and on screen are enabled.
	 * Note that if the log on file is enabled, it means that the
	 * logger has been already configured, therefore the stream is
	 * already open.
	 */
	loggerConf_ configuration_;

	/**
	 * \brief Stream used when logging on a file
	 */
	std::ofstream out_;

	/**
	 * \brief Initial time (used to print relative times)
	 */
	int64_t initialTime_usec_;

	/**
	 * \brief Verbosity threshold for files
	 */
	unsigned int fileVerbosityLevel_;

	/**
	 * \brief Verbosity threshold for screen
	 */
	unsigned int screenVerbosityLevel_;

	Logger();
	~Logger();

	/**
	 * \brief Method to lock in case of multithreading
	 */
	inline static void lock();

	/**
	 * \brief Method to unlock in case of multithreading
	 */
	inline static void unlock();

public:

	/**
	* Logging level
	* \enum Level
	* \public
	*/
	enum Level {
		EError = 0,
		EWarning,
		EMessage,
		EInfo,
		ETrace,
		EDebug
	};

	/**
	 * Macro to create and configure logger.
	 * \def DEBUG_CONF
	 *
	 * Example of configuration of the Logger:
	 * \code
	 *      DEBUG_CONF("outputfile", Logger::file_on|Logger::screen_on, DBG_DEBUG, DBG_ERROR);
	 * \endcode
	 */
	#define DEBUG_CONF(outputFile, \
			configuration, \
			fileVerbosityLevel, \
			screenVerbosityLevel) { \
				CUtil::Logger::getInstance().configure(outputFile, \
							configuration, \
							fileVerbosityLevel, \
							screenVerbosityLevel); \
			}

	/**
	 * \brief Macro to print log messages.
	 * \def LOGGER
	 *
	 * Example of usage of the Logger:
	 * \code
	 *      LOGGER(DBG_DEBUG, "hello " << "world");
	 * \endcode
	 */
	#define LOGGER(priority, msg) { \
		std::ostringstream __debug_stream__; \
		__debug_stream__ << msg; \
		CUtil::Logger::getInstance().print(priority, __FILE__, __LINE__, \
				__debug_stream__.str()); \
		}

	#ifndef _LOGGER_NO_LOG

	/**
	 * Macro to log errors.
	 * \def LOG_ERROR
	 */
	#define LOG_ERROR(M)        LOGGER(CUtil::Logger::EError, M)
	/**
	 * Macro to log warnings.
	 * \def LOG_WARNING
	 */
	#define LOG_WARNING(M)      LOGGER(CUtil::Logger::EWarning, M)
	/**
	 * Macro to log messages.
	 * \def LOG_MESSAGE
	 */
	#define LOG_MESSAGE(M)      LOGGER(CUtil::Logger::EMessage, M)
	/**
	 * Macro to log info messages.
	 * \def LOG_INFO
	 */
	#define LOG_INFO(M)         LOGGER(CUtil::Logger::EInfo, M)
	/**
	 * Macro to log trace messages.
	 * \def LOG_TRACE
	 */
	#define LOG_TRACE(M)        LOGGER(CUtil::Logger::ETrace, M)
	/**
	 * Macro to log debug messages.
	 * \def LOG_DEBUG
	 */
	#define LOG_DEBUG(M)        LOGGER(CUtil::Logger::EDebug, M)

	#else

	#define LOG_ERROR(M)        {}
	#define LOG_WARNING(M)      {}
	#define LOG_MESSAGE(M)      {}
	#define LOG_INFO(M)         {}
	#define LOG_TRACE(M)        {}
	#define LOG_DEBUG(M)        {}

	#endif

	/**
	 * Type used for the configuration
	 */
	typedef loggerConf_ loggerConf;
	/**
	 * Disable logging to file
	 */
	static const loggerConf file_on= 	L_nofile_;
	/**
	 * Enable logging to file
	 */
	static const loggerConf file_off= 	L_file_;
	/**
	 * Enable logging to screen
	 */
	static const loggerConf screen_on= 	L_noscreen_;
	/**
	 * Disable logging to screen
	 */
	static const loggerConf screen_off= L_screen_;

	static Logger& getInstance();

	void print(const unsigned int		verbosityLevel,
			const std::string&	sourceFile,
			const int 		codeLine,
			const std::string& 	message);

	void configure (const std::string&	outputFile,
			const loggerConf	configuration,
			const int		fileVerbosityLevel,
			const int		screenVerbosityLevel);

	/**
	 * Flush output buffer
	 */
	inline void flush()
	{
		out_.flush();
	}
};

/**
 * operator| - Can be used in Logger::configure
 * \param __a Configuration (i.e., log on file and on screen on or off).
 * \param __b Configuration (i.e., log on file and on screen on or off).
 * \return Configuration (i.e., log on file and on screen on or off).
 */
inline Logger::loggerConf operator|
	(Logger::loggerConf __a, Logger::loggerConf __b)
{
	return Logger::loggerConf(static_cast<int>(__a) |
		static_cast<int>(__b));
}

/**
 * operator& - Can be used in Logger::configure
 * \param __a Configuration (i.e., log on file and on screen on or off).
 * \param __b Configuration (i.e., log on file and on screen on or off).
 * \return Configuration (i.e., log on file and on screen on or off).
 */
inline Logger::loggerConf operator&
	(Logger::loggerConf __a, Logger::loggerConf __b)
{
	return Logger::loggerConf(static_cast<int>(__a) &
		static_cast<int>(__b)); }

}

#endif /* LOGGER_H */

/** @} */
