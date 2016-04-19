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

#include "sqlitedatabase.h"
#include <stdio.h>
#include <sqlite3.h>


bool
sqlitedatabase::init(const std::string & d)
{
	setDatabase(d);
	return true;
}

sqlitedatabase::~sqlitedatabase()
{
	sqlite3_close((sqlite3 *)m_odb->db);
	delete m_odb;
}

bool sqlitedatabase::Connected()
{
	SqliteDB * odb = grabdb();
	if(!odb)
	{
		return false;
	}
	freedb(odb);
	return true;
}

void sqlitedatabase::freedb(SqliteDB * odb)
{
	if(odb)
	{
		odb->busy = false;
	}
}

SqliteDB * sqlitedatabase::grabdb()
{
	SqliteDB * odb = NULL;

	if(!odb)
	{
		odb = new SqliteDB;
		if(!odb)
		{
			printf("grabdb: SqliteDB struct couldn't be created");
			return NULL;
		}
		void * p = &odb->db;
		if(sqlite3_open(database.c_str(), (sqlite3 **)p))
		{
			printf("Can't open database: %s\n", sqlite3_errmsg((sqlite3 *)odb->db));
			sqlite3_close((sqlite3 *)odb->db);
			delete odb;
			return NULL;
		}
		odb->busy = true;
	}
	else
	{
		odb->busy = true;
	}
	m_odb = odb;

	return odb;
}


