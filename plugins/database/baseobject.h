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

#ifndef _BASEOBJECTDAO_H_
#define _BASEOBJECTDAO_H_

#include "basedb.hpp"

#include<string>
#include<vector>

class BaseObject
{
public:
	BaseObject():id(-1){}
	virtual ~BaseObject(){}
	int id;
};

#endif

