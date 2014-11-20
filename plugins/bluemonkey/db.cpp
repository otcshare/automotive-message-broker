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
