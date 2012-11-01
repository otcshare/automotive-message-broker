/*
 * timedate - Displays time and date and daily events
 * Copyright (c) <2009>, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "utils.h"
#include <stdlib.h>

using namespace std;

string 
DaoUtils::findReplace(string name, string tofind, string replacewith, string exclusions)
{
	uint i=0;
	
	uint exclusionPos = exclusions.find(tofind,0);
	
	while(1)
	{
		i = name.find(tofind,i);
		
		if(i != string::npos && exclusionPos != string::npos)
		{
			if(name.substr(i-exclusionPos,exclusions.length()) == exclusions)
			{
				i+=replacewith.size();
				continue;
			}
		}
		
		if(i == string::npos)
			break;
		name.replace(i,tofind.size(),replacewith);
		i+=replacewith.size();
	}
	return name;
}

