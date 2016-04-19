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

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QVariantList>

class Database;
class Query;

class Database: public QObject
{
	Q_OBJECT
public:
	Database(QObject* parent = nullptr):QObject(parent){ }
	~Database() { close(); }

	QSqlDatabase database() { return db; }

public Q_SLOTS:

	bool open(QString connectionName, QString filename);
	void close();

	QObject* exec(QString query);

	QString lastError();

private:
	QSqlDatabase db;
};

class Query: public QObject
{
	Q_OBJECT
public:
	Query(QObject* parent = nullptr):QObject(parent){}
	Query(QSqlQuery q, QObject* parent = nullptr);
	Q_INVOKABLE Query(QString connectionName);
public Q_SLOTS:
	void setConnectionName(QString connectionName);

	bool exec(QString queryStr);

	QVariantList results();

private:
	QSqlQuery query;
	QString mConnectionName;
	QSqlDatabase db;
};


class BluemonkeyDatabaseModule : public QObject
{
	Q_OBJECT
public:
	BluemonkeyDatabaseModule(QObject* parent = nullptr): QObject(parent) { }

public Q_SLOTS:
	QObject* createNewDatabase()
	{
		return new Database(this);
	}

	QObject* createNewQuery()
	{
		return new Query(this);
	}
};
