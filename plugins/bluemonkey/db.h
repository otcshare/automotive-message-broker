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
