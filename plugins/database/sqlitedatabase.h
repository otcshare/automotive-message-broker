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

#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include <string>
#include <pthread.h>
#include <stdint.h>
#include <list>


struct SqliteDB
{
	SqliteDB() : busy(false){}
	void * db;
	bool busy;
};	// struct SqliteDB

class sqlitedatabase
{
public:
	sqlitedatabase() {}
	virtual ~sqlitedatabase();
		
	bool init(const std::string & d);

	virtual void setHost(const std::string &){}
	virtual void setUser(const std::string &){}
	virtual void setPassword(const std::string &){}
	virtual void setDatabase(const std::string & db){database = db;}

	virtual void OnInit(SqliteDB *){}
	virtual bool Connected();
	virtual SqliteDB * grabdb();
	virtual void freedb(SqliteDB * odb);
	
protected:
	SqliteDB *m_odb;
	bool m_embedded;
	std::string database;
};

#endif

