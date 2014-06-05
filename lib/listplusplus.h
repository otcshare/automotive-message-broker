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


#ifndef LISTPLUSPLUS_H
#define LISTPLUSPLUS_H

#include <list>
#include <algorithm>

template <class T, class V>
bool contains(const T & iteratable, V value)
{
	return iteratable.size() > 0 && (std::find(iteratable.begin(), iteratable.end(), value) != iteratable.end());
}

template <class T, class V>
void removeOne(T * iteratable, V value)
{
	typename T::iterator itr = std::find(iteratable->begin(), iteratable->end(), value);

	if (itr != iteratable->end())
	{
		iteratable->erase(itr);
	}
}

#endif // LISTPLUSPLUS_H
