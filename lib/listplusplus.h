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
#include <functional>

template <class T, class V, class Predicate>
bool contains(const T & iteratable, V value, Predicate comparator )
{
	return std::any_of(iteratable.begin(), iteratable.end(), [value, &comparator](auto i) { return comparator(value, i); });
}

template <class T, class V>
bool contains(const T & iteratable, V value)
{
	return contains(iteratable, value, [](auto a, auto b) { return a == b; });
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

namespace amb
{
template <class T, class Key>
bool containsKey(const T & map, const Key & key)
{
	return map.find(key) != map.end();
}

std::string::size_type count(const std::string & t, const std::string & toFind, const std::string & before = "");

int findNth(const std::string & t, const std::string &toFind, std::string::size_type n);

}

#endif // LISTPLUSPLUS_H
