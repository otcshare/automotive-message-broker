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

#include "db.h"

#include <debugout.h>

#include <QObject>
#include <QSqlError>
#include <QSqlRecord>
#include <QtQml>

extern "C" std::map<std::string, QObject*> create(std::map<std::string, std::string> config, QObject* parent)
{
	std::map<std::string, QObject*> moduleInstances;
	moduleInstances["database"] = new BluemonkeyDatabaseModule(parent);
	return moduleInstances;
}

bool Database::open(QString connectionName, QString filename)
{
	db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
	db.setDatabaseName(filename);
	return db.open();
}

void Database::close()
{
	if(db.isOpen())
	{
		db.close();
	}
}

QObject *Database::exec(QString query)
{
	return new Query(db.exec(query), this);
}

QString Database::lastError()
{
	return db.lastError().text();
}


Query::Query(QSqlQuery q, QObject* parent)
	:QObject(parent)
{
	query = q;
}

Query::Query(QString connectionName)
{
	setConnectionName(connectionName);
}

void Query::setConnectionName(QString connectionName)
{
	mConnectionName = connectionName;
	db = QSqlDatabase::database(connectionName);
	query = QSqlQuery(db);
}

bool Query::exec(QString queryStr)
{
	query.setNumericalPrecisionPolicy(QSql::HighPrecision);
	return query.exec(queryStr);
}

QVariantList Query::results()
{
	QVariantList results;

	QSqlRecord r = query.record();

	while(query.next())
	{

		QVariantMap prop;
		///iterate on the properties:
		for(int i=0; i< r.count(); i++)
		{
			QString name = r.fieldName(i);

			QVariant val = query.value(i);
				prop[name] = val;
		}

		results << prop;
	}

	return results;
}
