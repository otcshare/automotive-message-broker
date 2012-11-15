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

#ifndef _BASEDB_H_
#define _BASEDB_H_

#include "sqlitedatabase.h"
#include "sqlitequery.h"
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

template<typename T>
class NameValuePair
{
	public:
		NameValuePair(){ }
		NameValuePair(std::string n, T v){name = n; value = v;}
		std::string name;
		T value;
};

template<typename T>
class DictionaryList: public std::vector<NameValuePair<T> >
{

};

class BaseDB
{
public:
	BaseDB():db(NULL),q(NULL)
	{
		
	}
	
	
	virtual ~BaseDB()
	{
		printf("BaseDB: Destroying db object. Table: %s",table.c_str());
		delete q;
		delete db;
	}
	
	void setTable(string tablename)
	{
		if(tablename == "") return;
		table = tablename;
		
		if(!tableExists())
			reloadTable();
	}
	
	virtual void
	init(string dbname, string tablename, string tablestring)
	{
		printf("BaseDB: Initializing db object. Table: %s",tablename.c_str());
		tableString = tablestring;
		
		db = new sqlitedatabase();
		
		db->init(dbname);
		
		printf("BaseDB: Using db/db-file: %s",dbname.c_str());
		
		if(! db->Connected())
		{
			printf("BaseDB: database not found %s",dbname.c_str());
		}
		q = new sqlitequery();;
		
		q->init(db);

		setTable(tablename);
	}
	
	virtual void
	reloadTable()
	{
		printf("BaseDB: reloading table %s",table.c_str());
		dropTable();
		createTable();
	}
	
	virtual bool tableExists()
	{
		bool exists=false;
		string query = "SELECT * FROM "+table+" LIMIT 0,1";
		printf("BaseDB: checking for existing table with %s",query.c_str());
		q->getResult(query);
		int numrows = q->numRows();
		if(numrows <= 0 )
			exists = false;
		else exists = true;

		printf("BaseDB: Table '%s' exists? %d because %d rows where found.", table.c_str(), exists, numrows);
		q->freeResult();
		return exists;
	}
	
	virtual void
	renameTable(string newname)
	{
		dropTable(newname);
		string query = "ALTER TABLE "+table+" RENAME TO "+newname;
		q->execute(query);
	}
	
	template<typename T>
	void insert(DictionaryList<T> params)
	{
		string query = "INSERT INTO "+table+" (";
		ostringstream endquery;
		endquery<<" VALUES ( ";
		for(size_t i=0; i< params.size(); i++)
		{
			query+=" `"+fixInvalids(params[i].name)+"`";
			ostringstream tempval;
			tempval<<params[i].value;
			endquery<<"'"<<fixInvalids(tempval.str())<<"'";
			if(i < params.size()-1)
			{
				query+=",";	
				endquery<<",";
			}
		}
		endquery<<" )";
		query+=" )"+endquery.str();
		printf("BaseDB: %s",query.c_str());
		q->execute(query);
	}
	
	template<typename T>
	void
	insert(NameValuePair<T> param)
	{
		string query = "INSERT INTO "+table+" (";
		ostringstream endquery;
		endquery<<" VALUES ( ";
		query+=" `"+fixInvalids(param.name)+"`";
		ostringstream tempval;
		tempval<<param.value;
		endquery<<"'"<<fixInvalids(tempval.str())<<"'";
		endquery<<" )";
		query+=" )"+endquery.str();
		printf("BaseDB: %s",query.c_str());
		q->execute(query);
	}
	
	virtual void
	insert(DictionaryList<string> params)
	{
		insert<string>(params);
	}
	
	template<typename T, typename TT, typename T3>
	void
	update(T col, TT colval, NameValuePair<T3> qualifier)
	{
		ostringstream query;
		ostringstream tempval;
		ostringstream tempcolval;
		tempval<<qualifier.value;
		tempcolval<<colval;
		query << "UPDATE "<< table <<
			" SET `"<<col<<"` = '"<<fixInvalids(tempcolval.str())<<
			"' WHERE `"<<fixInvalids(qualifier.name)<<"` = '"<<fixInvalids(tempval.str())<<"'";
		printf("BaseDB: Update: %s",query.str().c_str());
		q->execute(query.str());
	}
	
	template<typename T, typename TT>
	void 
	update(NameValuePair<T> param, NameValuePair<TT> qualifier)
	{
		update<string,T,TT>(param.name, param.value, qualifier);
	}
	
	template<typename T, typename TT>
	void
	update(DictionaryList<T> params, NameValuePair<TT> qualifier)
	{
		for(size_t i=0;i<params.size();i++)
		{
			update<T,TT>(params[i],qualifier);
		}
	}
	
	virtual void update(NameValuePair<string> param, NameValuePair<string> qualifier)
	{
		update<string,string>(param,qualifier);
	}
	
	template<typename T>
	void deleteRow(NameValuePair<T> qualifier)
	{
		ostringstream query;
		ostringstream tempval;
		tempval<<qualifier.value;
		query << "DELETE FROM "<< table<<
				" WHERE `"<<qualifier.name<<"` = '"<<fixInvalids(tempval.str())<<"'";
		printf("BaseDB: %s: %s",__FUNCTION__, query.str().c_str());
		q->execute(query.str());
	}
	
	virtual void 
	deleteRow(NameValuePair<string> qualifier)
	{
		deleteRow<string>(qualifier);
	}
	
	virtual void
	dropTable()
	{
		dropTable(table);
	}
	
	virtual void
	dropTable(string tablename)
	{
		string query="DROP TABLE IF EXISTS "+tablename;
		printf("BaseDB: Dropping Table %s with query:? %s",tablename.c_str(),query.c_str());
		q->execute(query);
	}
	
	virtual void
	createTable()
	{
		string t = tableString;
		string query;
		string::size_type i=t.find("%s",0);
		if(i!=string::npos) query=t.replace(i, 2, table);
		else query = t;
		printf("BaseDB: Creating Table %s with query:? %s",table.c_str(),query.c_str());
		q->execute(query);
	}
	

	
	string
	fixInvalids(string filename)
	{
		return filename;
	}
	
protected:
	
	void
	fixFilename(string* filename)
	{
		std::string::size_type i=0;
		while(1)
		{
			i = filename->find(" ",i);
			if(i == string::npos)
				break;
			filename->replace(i,1,"\\ ");
			i+=2;
		}
	}

	void
	unfixFilename(string* filename)
	{
		std::string::size_type i=0;
		i=filename->find("\\",0);
		if(i == string::npos)
			return;
		else
		{
			filename->replace(i,1,"");
			unfixFilename(filename);
		}
		
	}
		
	sqlitedatabase *db;
	sqlitequery *q;
	string table;
	string tableString;

}; //BaseDB class

#endif

