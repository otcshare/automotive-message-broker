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

#include <iostream>
#include <new>
#include <cstdlib>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>


#include "logger.h"

using namespace CUtil;

// Definition (and initialization) of static attributes
Logger* Logger::m_ = 0;

// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void signal_callback_handler(int signum)
{
    std::cerr << std::endl << " ... Caught signal " << signum << std::endl;

    Logger::getInstance().flush();

    // Terminate program
    exit(signum);
}


#ifdef LOGGER_MULTITHREAD
pthread_mutex_t Logger::lock_ = PTHREAD_MUTEX_INITIALIZER;
inline void Logger::lock()
{
	pthread_mutex_lock(&lock_);
}

inline void Logger::unlock()
{
	pthread_mutex_unlock(&lock_);
}
#else
void Logger::lock(){}
void Logger::unlock(){}
#endif



/**
 * \brief Constructor.
 * It is a private constructor, called only by getInstance() and only the
 * first time. It is called inside a lock, so lock inside this method
 * is not required.
 * It only initializes the initial time. All configuration is done inside the
 * configure() method.
 */
Logger::Logger() :
        configured_(false),
        logFile_(""),
        logFileName_(""),
        configuration_(L_nofile_|L_noscreen_),
        initialTime_usec_(0)
{
	struct timeval initialTime;
	gettimeofday(&initialTime, NULL);
	initialTime_usec_ = int64_t(1000000LL*int64_t(initialTime.tv_sec)) + int64_t(initialTime.tv_usec);

}

/**
 * \brief Method to configure the logger. Called by the DEBUG_CONF() macro.
 * To make implementation easier, the old stream is always closed.
 * Then, in case, it is open again in append mode.
 * @param outputFile Name of the file used for logging.
 * @param configuration Configuration (i.e., log on file and on screen on or off).
 * @param fileVerbosityLevel Verbosity threshold for file.
 * @param screenVerbosityLevel Verbosity threshold for screen.
 */
void Logger::configure (const std::string&	outputFile,
			const loggerConf	configuration,
			const int		fileVerbosityLevel,
			const int		screenVerbosityLevel)
{
    Logger::lock();

    logFileName_ = outputFile;
    fileVerbosityLevel_ = fileVerbosityLevel;
    screenVerbosityLevel_ = screenVerbosityLevel;

    // Close the old stream, if needed
    if (configuration_&file_on)
        out_.close();

    // Compute a new file name, if needed
    if (outputFile != logFile_){
        std::ostringstream oss;
        /*time_t currTime;
        time(&currTime);
        struct tm *currTm = localtime(&currTime);*/
        oss << outputFile << /*"_" <<
                (1900 + currTm->tm_year) << "_" <<
                currTm->tm_mon << "_" <<
                currTm->tm_mday << "_" <<
                currTm->tm_hour << "-" <<
                currTm->tm_min << "-" <<
                currTm->tm_sec << */
                ".log";
        logFile_ = oss.str().c_str();
    }

    // Open a new stream, if needed
    if (configuration&file_on)
        out_.open(logFile_.c_str(), std::ios::app);

    configuration_ = configuration;
    configured_ = true;

    Logger::unlock();

    // Register signal and signal handler
    // we need a flush log file at program termination
    signal(SIGINT, signal_callback_handler);
    signal(SIGTERM, signal_callback_handler);
    signal(SIGABRT, signal_callback_handler);
    signal(SIGKILL, signal_callback_handler);

}

/**
 * \brief Destructor.
 * It only closes the file, if open, and cleans memory.
 */

Logger::~Logger()
{
	Logger::lock();
	if (configuration_&file_on)
		out_.close();
	delete m_;
	Logger::unlock();

}

/**
 * \brief Method to get a reference to the object (i.e., Singleton)
 * It is a static method.
 * @return Reference to the object.
 */
Logger& Logger::getInstance()
{
	Logger::lock();
	if (m_ == 0)
		m_ = new Logger;
	Logger::unlock();
	return *m_;
}


/**
 * \brief Method used to print messages.
 * Called by the LOGGER() macro.
 * @param verbosityLevel Priority of the message
 * @param file Source file where the method has been called (set equal to __FILE__
 * 	      by the LOGGER macro)
 * @param line Source line where the method has been called (set equal to __LINE__
          by the macro)
 * @param message Message text
 */
void Logger::print(const unsigned int verbosityLevel,
					const std::string& file,
					const int line,
					const std::string& message)
{
	if (!configured_) {
			std::cerr << "ERROR: Logger not configured!" << 
				std::endl;
			return;
	}

	if( ((configuration_&file_on) && (verbosityLevel > fileVerbosityLevel_)) &&
	        ((configuration_&screen_on) && (verbosityLevel > screenVerbosityLevel_)) )
		return;

	struct timeval currentTime;
	Logger::lock();

	gettimeofday(&currentTime, NULL);
	int64_t currentTime_usec;
	currentTime_usec =  int64_t(1000000LL*int64_t(currentTime.tv_sec)) + int64_t(currentTime.tv_usec);

	unsigned int li_tid = (unsigned int)pthread_self();

	if ( (configuration_&file_on) && (verbosityLevel <= fileVerbosityLevel_) )
			out_ << double(currentTime_usec - initialTime_usec_)/1000000.0f <<
				" (" << std::hex << li_tid << std::dec << ") " <<
				" [" << file << ":" << line << "] @ " <<
				message << std::endl;

	if ( (configuration_&screen_on) && (verbosityLevel <= screenVerbosityLevel_) )
			std::cout <<
				double(currentTime_usec - initialTime_usec_)/1000000.0f <<
				" (" << std::hex << li_tid << std::dec << ") " <<
				logFileName_ << " [" << file << ":" << line << "] @ " <<
				message << std::endl;


	Logger::unlock();
}


