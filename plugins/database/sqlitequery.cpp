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

#include "sqlitequery.h"
#include <stdio.h>
#include <sqlite3.h>
#include <string>
#include "utils.h"

using namespace std;

bool
sqlitequery::init(sqlitedatabase * db)
{
	m_db = db;
	odb = (db ? db->grabdb() : NULL);
	return true;
}

bool
sqlitequery::init(sqlitedatabase * dbin, const std::string & sql)
{
	init(dbin);
	return execute(sql);
}

sqlitequery::~sqlitequery()
{
	if(result)
	{
		printf( "sqlite3_finalize in destructor\n");
		sqlite3_finalize(result);
	}
	if(odb)
	{
		m_db->freedb(odb);
	}
}

bool sqlitequery::execute(const std::string & sql)
{
	std::string sql_sqlite=sql;
	m_last_query = sql_sqlite;
	if(odb && result)
	{
		string err = "execute: query busy: "+sql_sqlite;
		printf("%s\n", err.c_str());
	}
	if(odb && !result)
	{
		const char * s = NULL;
		int err = sqlite3_busy_timeout((sqlite3 *)odb->db, 10000 );

		if(err!= SQLITE_OK)
		{
			printf("execute: busy timeout occured: \n");
			return false;
		}
		int rc = sqlite3_prepare_v2((sqlite3 *)odb->db, sql_sqlite.c_str(), sql_sqlite.size(), &result, &s);
		
		if(rc != SQLITE_OK)
		{
			string err = "execute: prepare query failed: "+sql_sqlite;
			printf("%s\n", err.c_str());
			result = NULL;
			return false;
		}
		if(!result)
		{
			printf( "execute: query failed\n");
			result = NULL;
			return false;
		}
		rc = sqlite3_step(result);
		sqlite3_finalize(result);
		result = NULL;
		switch(rc)
		{
		case SQLITE_BUSY:
			printf( "execute: database busy\n");
			return false;
		case SQLITE_DONE:
		case SQLITE_ROW:
			return true;
		case SQLITE_SCHEMA:
			printf( "execute: Schema error\n");
			return false;
		case SQLITE_ERROR:
			printf("%s\n", sqlite3_errmsg((sqlite3 *)odb->db));
			return false;
		case SQLITE_MISUSE:
			printf( "execute: database misuse\n");
			return false;
		default:
			printf( "execute: unknown result code\n");
		}
	}
	return false;
}

bool sqlitequery::fetchRow()
{
	rowcount = 0;
	row = false;
	if(odb && result)
	{
		int rc = cache_rc_valid ? cache_rc : sqlite3_step(result);
		cache_rc_valid = false;
		switch(rc)
		{
		case SQLITE_BUSY:
			printf( "execute: database busy\n");
			return false;
		case SQLITE_DONE:
			return false;
		case SQLITE_ROW:
			row = true;
			return true;
		case SQLITE_ERROR:
			printf("%s\n", sqlite3_errmsg((sqlite3 *)odb->db));
			return false;
		case SQLITE_MISUSE:
			printf( "execute: database misuse\n");
			return false;
		default:
			printf( "execute: unknown result code\n");
		}
	}
	return false;
}

void sqlitequery::freeResult()
{
	if(odb && result)
	{
		sqlite3_finalize(result);
		result = NULL;
		row = false;
		cache_rc_valid = false;
	}
	while(m_nmap.size())
	{
		std::map<std::string, int>::iterator it = m_nmap.begin();
		m_nmap.erase(it);
	}
}

void sqlitequery::resetStatement()
{
	if( odb && result )
		sqlite3_reset(result);
}

long sqlitequery::getCount(const std::string & sql)
{
	long l(0);
	if(getResult(sql))
	{
		if(fetchRow())
		{
			l = getVal(rowcount++);
		}
		freeResult();
	}
	return l;
}

bool sqlitequery::prepareStatement(const std::string & sql)
{
	std::string sql_sqlite=sql;
	m_last_query = sql_sqlite;
	if(odb && result)
	{
		string err = "prepareStatement: query busy: "+sql_sqlite;
		printf("%s\n", err.c_str());
	}
	if(odb && !result)
	{
		const char * s = NULL;
		int rc = sqlite3_prepare_v2((sqlite3 *)odb->db, sql_sqlite.c_str(), sql_sqlite.size(), &result, &s);
		if(rc != SQLITE_OK)
		{
			string err = "prepareStatement: prepare query failed: "+sql_sqlite;
			printf("%s\n", err.c_str());
			return false;
		}
		if(!result)
		{
			printf( "prepareStatement: query failed\n");
			return false;
		}
	}
	return result;
}

bool sqlitequery::getResult(const std::string & sql)
{
	if(prepareStatement(sql))
	{
		int i(0);
		const char * p = sqlite3_column_name(result, i);
		while(p)
		{
			m_nmap[p] = ++i;
			p = sqlite3_column_name(result, i);
		}
		m_num_cols = i;
		cache_rc = sqlite3_step(result);
		cache_rc_valid = true;
		m_row_count = (cache_rc == SQLITE_ROW) ? 1 : 0;
	}
	return result;
}

bool sqlitequery::bind(const std::string bindMatch)
{
	if(!odb || !result) return false;
	
	int r = sqlite3_bind_text(result,1,bindMatch.c_str(),bindMatch.length(),NULL);
	
	if(r != SQLITE_OK)
	{
		printf("sqlitequery::bind - error binding query\n");
		return false;
	}
	
	int i(0);
	const char * p = sqlite3_column_name(result, i);
	while(p)
	{
		m_nmap[p] = ++i;
		p = sqlite3_column_name(result, i);
	}
	m_num_cols = i;
	cache_rc = sqlite3_step(result);
	cache_rc_valid = true;
	m_row_count = (cache_rc == SQLITE_ROW) ? 1 : 0;
	
	return true;
}

bool sqlitequery::bind(const int value)
{
	if(!odb || !result) return false;
	
	int r = sqlite3_bind_int(result,1,value);
	
	if(r != SQLITE_OK)
	{
		printf("sqlitequery::bind - error binding query\n");
		return false;
	}
	
	int i(0);
	const char * p = sqlite3_column_name(result, i);
	while(p)
	{
		m_nmap[p] = ++i;
		p = sqlite3_column_name(result, i);
	}
	
	m_num_cols = i;
	cache_rc = sqlite3_step(result);
	cache_rc_valid = true;
	m_row_count = (cache_rc == SQLITE_ROW) ? 1 : 0;
	
	return true;
}

bool sqlitequery::bind(const double value)
{
	if(!odb || !result) return false;
	
	int r = sqlite3_bind_double(result,1,value);
	
	if(r != SQLITE_OK)
	{
		printf("sqlitequery::bind - error binding query\n");
		return false;
	}
	
	int i(0);
	const char * p = sqlite3_column_name(result, i);
	while(p)
	{
		m_nmap[p] = ++i;
		p = sqlite3_column_name(result, i);
	}
	m_num_cols = i;
	cache_rc = sqlite3_step(result);
	cache_rc_valid = true;
	m_row_count = (cache_rc == SQLITE_ROW) ? 1 : 0;
	
	return cache_rc == SQLITE_OK;
}

int64_t sqlitequery::getBigInt(int x)
{
	if(odb && result && row)
	{
		return sqlite3_column_int64(result, x);
	}
	return 0;
}

int sqlitequery::GetErrno()
{
	if(odb)
	{
		return sqlite3_errcode((sqlite3 *)odb->db);
	}
	return 0;
}

std::string sqlitequery::GetError()
{
	if(odb)
	{
		return sqlite3_errmsg((sqlite3 *)odb->db);
	}
	return "";
}

double sqlitequery::getNum(int x)
{
	if(odb && result && row)
	{
		return sqlite3_column_double(result, x);
	}
	return 0;
}

const char * sqlitequery::getStr(int x)
{
	if(odb && result && row && x < sqlite3_column_count(result))
	{
		const unsigned char * tmp = sqlite3_column_text(result, x);
		return tmp ? (const char *)tmp : "";
	}
	return "";
}

uint64_t sqlitequery::getUBigInt(int x)
{
	if(odb && result && row)
	{
		return (uint64_t)sqlite3_column_int64(result, x);
	}
	return 0;
}

unsigned long sqlitequery::getUVal(int x)
{
	if(odb && result && row)
	{
		return (unsigned long)sqlite3_column_int(result, x);
	}
	return 0;
}

long sqlitequery::getVal(int x)
{
	if(odb && result && row)
	{
		return sqlite3_column_int(result, x);
	}
	return 0;
}

bool sqlitequery::isNull(int x)
{
	if(odb && result && row)
	{
		if(sqlite3_column_type(result, x) == SQLITE_NULL)
		{
			return true;
		}
	}
	return false;
}

long sqlitequery::numRows()
{
	return odb && result ? m_row_count : 0;
}

