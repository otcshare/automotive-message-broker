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
#include "debugout.h"
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
		DebugOut()<<"BaseDB: Destroying db object. Table: "<<table<<endl;
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
		DebugOut()<<"BaseDB: Initializing db object. Table: "<<tablename.c_str()<<endl;
		tableString = tablestring;

		db = new sqlitedatabase();

		db->init(dbname);

		DebugOut()<<"BaseDB: Using db/db-file: "<<dbname.c_str()<<endl;

		if(! db->Connected())
		{
			DebugOut(0)<<"BaseDB: database not found "<<dbname<<endl;
			return;
		}
		q = new sqlitequery();

		q->init(db);

		setTable(tablename);
	}

	virtual void
	reloadTable()
	{
		DebugOut()<<"BaseDB: reloading table "<<table<<endl;
		dropTable();
		createTable();
	}

	virtual bool tableExists()
	{
		bool exists=false;
		string query = "SELECT * FROM "+table+" LIMIT 0,1";
		DebugOut()<<"BaseDB: checking for existing table with "<<query.c_str()<<endl;
		q->getResult(query);
		int numrows = q->numRows();
		if(numrows <= 0 )
			exists = false;
		else exists = true;

		DebugOut()<<"BaseDB: Table '"<<table<<"' exists? "<<exists<<" because "<<numrows<<" rows where found."<<endl;
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
		DebugOut()<<"BaseDB: "<<query<<endl;
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
		DebugOut()<<"BaseDB: "<<query<<endl;
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
		DebugOut() << "BaseDB: Update: " << query.str() << endl;
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
		DebugOut() << "BaseDB: " << __FUNCTION__ << " : " << query.str() << endl;
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
		DebugOut() << "BaseDB: Dropping Table " << tablename <<" with query: " << query << endl;
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
		DebugOut() << "BaseDB: Creating Table" << table << " with query: " << query << endl;
		q->execute(query);
	}



	string
	fixInvalids(string filename)
	{
		return filename;
	}

	vector<vector<string> > select(string query)
	{
		DebugOut()<<query<<endl;

		vector<vector<string>> dataMap;

		q->getResult(query);

		if(q->numRows() <= 0)
		{
			q->freeResult();
			return dataMap;
		}

		int i=0;

		while(q->fetchRow())
		{
			string v;
			dataMap.push_back(vector<string>());

			while((v = q->getStr()) != "")
			{
				dataMap[i].push_back(v);
			}
			i++;
		}

		q->freeResult();

		return dataMap;

	}

	void exec(string query)
	{
		q->execute(query);
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

