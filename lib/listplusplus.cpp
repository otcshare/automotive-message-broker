/*
	Copyright (C) 2012  Intel Corporation

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


#include "listplusplus.h"
#include "debugout.h"

std::string::size_type amb::count(const std::string & t, const std::string & toFind, const std::string &before)
{
	int count = 0;
	size_t pos = -1;

	std::string::size_type beforePos = t.find(before);

	while((pos = t.find(toFind, pos+1)) != std::string::npos && (before.empty() || pos < beforePos))
	{
		count++;
	}

	return count;
}

int amb::findNth(const std::string & t, const std::string & toFind, std::string::size_type n)
{
	size_t count = 0;
	auto itr = t.begin();
	for(; count < n; itr++, count++)
	{
		if(itr == t.end())
			break;

		itr = std::search(itr, t.end(), toFind.begin(), toFind.end());
	}

	if(count != n)
		return -1;


	return std::distance(t.begin(), itr) - 1;
}

