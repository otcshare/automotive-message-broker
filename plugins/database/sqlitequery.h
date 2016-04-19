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

#ifndef SQLITEQUERY_H
#define SQLITEQUERY_H

#include "sqlitedatabase.h"
#include <map>

class sqlite3_stmt;

class sqlitequery 
{
public:
	sqlitequery() :  result(NULL), row(false), cache_rc(0), cache_rc_valid(false), m_row_count(0), m_db(NULL),  m_num_cols(0)  {}
	virtual ~sqlitequery();

	bool init(sqlitedatabase * dbin);
	bool init(sqlitedatabase * dbin, const std::string & sql);

	virtual bool Connected(){return odb ? true : false;}
	virtual bool execute(const std::string & sql);
	virtual bool fetchRow();
	virtual long numRows();
	virtual void freeResult();
	virtual void resetStatement();
	virtual long getCount(const std::string & sql);
	virtual bool prepareStatement(const std::string & sql);
	virtual bool getResult(const std::string & sql);
	virtual bool bind(const std::string bindMatch);
	virtual bool bind(const int value);
	virtual bool bind(const double value);
	virtual int64_t getBigInt(){return getBigInt(rowcount++);}
	virtual int64_t getBigInt(int x);
	virtual int GetErrno();
	virtual std::string GetError();
	virtual double getNum(){ return getNum(rowcount++); }
	virtual double getNum(int x);
	virtual const char * getStr(){return getStr(rowcount++);}
	virtual const char * getStr(int x);
	virtual uint64_t getUBigInt(){ return getUBigInt(rowcount++); }
	virtual uint64_t getUBigInt(int x);
	virtual unsigned long getUVal(){ return getUVal(rowcount++); }
	virtual unsigned long getUVal(int x);
	virtual long getVal(){ return getVal(rowcount++); }
	virtual long getVal(int x);
	virtual bool isNull(int x);
protected:
	sqlite3_stmt * result;
	bool row;
	int cache_rc;
	bool cache_rc_valid;
	int m_row_count;
	
	virtual sqlitequery & operator=(const sqlitequery &){return *this;}

	sqlitedatabase * m_db;
	std::string m_last_query;
	short rowcount;
	std::string m_tmpstr;
	std::map<std::string, int> m_nmap;
	int m_num_cols;
	SqliteDB * odb;
	
private:
	std::string sql_replace_tokens(std::string sqlstring,std::string &dest);
};

#endif

