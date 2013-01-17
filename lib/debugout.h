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

#include "timestamp.h"

using namespace std;

void debugOut(string message);

class DebugOut 
{
public: 
	DebugOut(int debugLevel = 4)
	{
		mDebugLevel = debugLevel;
		ostream out(buf);

		out.precision(15);

		if(mDebugLevel <= debugThreshhold)
			out<<amb::currentTime()<<" | ";
	}

	DebugOut const& operator << (string message) const
	{
		ostream out(buf);

		out.precision(15);

		if(mDebugLevel <= debugThreshhold)
			 out<<message<<" ";
		return *this;
	}

	DebugOut const& operator << (ostream & (*manip)(std::ostream&)) const
	{
		ostream out(buf);

		out.precision(15);

		if(mDebugLevel <= debugThreshhold)
			 out<<endl;
		return *this;
	}
	
	/*DebugOut const & operator << (uint16_t val) const
	{
		ostream out(buf);

		if(mDebugLevel <= debugThreshhold)
			 out<<val<<" ";
		return *this;
	}*/

	DebugOut const & operator << (double val) const
	{
		ostream out(buf);

		out.precision(5);

		if(mDebugLevel <= debugThreshhold)
			 out<<val<<" ";
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

private:
	static int debugThreshhold;
	static std::streambuf *buf;
	int mDebugLevel;
};





#endif
