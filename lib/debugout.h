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

#ifndef DEBUGOUT__H__
#define DEBUGOUT__H__

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "timestamp.h"

using namespace std;

void debugOut(const string &message);

/*! \addtogroup libamb
 *  @{
 */

/*!
 * \file debugout.h
 * \brief The DebugOut class represents a class used for outputing debug information
 * The specified debug level will only be outputed if the debug level is => the debug threshhold
 * Here's a simple example:
 * \code
 * DebugOut::setDebugThreshhold(3);
 * DebugOut(DebugOut::Warning) << "This is a warning" << std::endl;
 * DebugOut(3) << "This will only show if the threshhold is 3 or lower." << std::endl;
 *
 * /// Start logging to a file:
 * ofstream logfile;
 * logfile.open("amb.log", ios::out | ios::trunc);
 * DebugOut::setOutput(logfile)
 *
 * /// Throw exception on warning or error:
 * DebugOut::setThrowErr(true);
 * DebugOut::setThrowWarn(true);
 * DebugOut(DebugOut::Error) << "This will throw an exception." << std::endl;
 *
 * /// Log to stderr:
 * DebugOut::setOutput(std::cerr);
 * DebugOut() << "This will log to stderr." << std::endl;
 * \endcode
 */

namespace amb
{
/*!
 * \brief deprecateMethod prints warning if method is used.  Throws if version >= PROJECT_SERIES
 * \param methodName name of method being deprecated.
 * \param version version in which this method will no longer function in.
 */
void deprecateMethod(const string &methodName, const std::string & version);
}

class DebugOut
{
public:

	/*!
	 * \brief Error use when essential functionality is blocked
	 */
	static const int Error;

	/*!
	 * \brief Warning use when non-essential functionality is bocked, or when workarounds exist.
	 */
	static const int Warning;

	DebugOut(int debugLevel = 4)
	{
		mDebugLevel = debugLevel;

		if(mDebugLevel <= debugThreshhold || mDebugLevel == Error || mDebugLevel == Warning)
		{
			ostream out(buf);
			out.precision(15);
			out<<bufferTime(amb::currentTime())<<" | ";

			if(mDebugLevel == Error)
				out<<"ERROR ";
			if(mDebugLevel == Warning)
				out<<"WARNING ";
		}
	}

	DebugOut(const std::string & toLog, int debugLevel = 4)
		:DebugOut(debugLevel)
	{
		(*this) << toLog << endl;
	}

	DebugOut const& operator << (const string &message) const
	{
		if(mDebugLevel <= debugThreshhold || mDebugLevel == Error || mDebugLevel == Warning)
		{
			ostream out(buf);
			out.precision(15);
			out<<message;
		}
		return *this;
	}

	DebugOut const& operator << (ostream & (*manip)(std::ostream&)) const
	{
		if(mDebugLevel <= debugThreshhold || mDebugLevel == Error || mDebugLevel == Warning)
		{
			ostream out(buf);
			out.precision(15);
			out<<endl;

			if((mDebugLevel == Error && throwErr))
			{
				throw std::runtime_error("Abort on Error is set");
			}
			else if ((mDebugLevel == Warning && throwWarn))
			{
				throw std::runtime_error("Abort on Warning is set");
			}
		}
		return *this;
	}

	DebugOut const & operator << (double val) const
	{
		if(mDebugLevel <= debugThreshhold || mDebugLevel == Error || mDebugLevel == Warning)
		{
			ostream out(buf);
			out.precision(15);
			out<<val;
		}
		return *this;
	}

	static void setDebugThreshhold(int th)
	{
		debugThreshhold = th;
	}

	static void setOutput(ostream &o)
	{
		buf = o.rdbuf();
	}

	static void setThrowWarn(bool v)
	{
		throwWarn = v;
	}

	static void setThrowErr(bool v)
	{
		throwErr = v;
	}

	static const int getDebugThreshhold()
	{
		return debugThreshhold;
	}

private:

	std::string bufferTime(double time)
	{
		ostringstream f;

		f.precision(15);

		f<<time;

		while(f.str().length() <= 15)
		{
			f<<" ";
		}

		return f.str();
	}

	static int debugThreshhold;
	static std::streambuf *buf;
	static bool throwWarn;
	static bool throwErr;
	int mDebugLevel;
};

#endif

/** @} */
