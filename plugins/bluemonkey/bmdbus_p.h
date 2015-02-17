#ifndef _BMDBUS_P_H_
#define _BMDBUS_P_H_

#include <QJSValue>
#include <QHash>

#include <private/qobject_p.h>
#include <private/qmetaobjectbuilder_p.h>

class BluemonkeyQObject;
class BluemonkeyMetaObject;
class BluemonkeyQObjectPrivate : public QObjectPrivate
{
public:
	BluemonkeyQObjectPrivate(BluemonkeyQObject* iface);

	virtual ~BluemonkeyQObjectPrivate() {}

	QVariant property(const QByteArray& property);
	bool setProperty(const QByteArray& property, const QVariant& value);

	void createFrom(const QJSValue & value);

	BluemonkeyQObject* q;
	BluemonkeyMetaObject* metaObject;
};

class BluemonkeyMetaObject : public QAbstractDynamicMetaObject
{
public:
	BluemonkeyMetaObject(BluemonkeyQObject* qq, BluemonkeyQObjectPrivate* dd,
						 const QMetaObject* mo);
	~BluemonkeyMetaObject();

	void updateProperties(const QVariantMap & data);

private:
	void addProperty(const QByteArray & name, const QJSValue & property);
	void addMethod(const QByteArray & name, const QString &returnType, const QStringList &args);

	QMetaType::Type jsType(const QJSValue& value);

protected:
	virtual int metaCall(QMetaObject::Call _c, int _id, void** _a);
	virtual int createProperty(const char *, const char *);

private:
	BluemonkeyQObject *q;
	BluemonkeyQObjectPrivate *d;

	QMetaObjectBuilder m_builder;
	QMetaObject* m_metaObject;
	QAbstractDynamicMetaObject* m_parent;

	int m_propertyOffset;
	int m_methodOffset;

	QHash<QByteArray, int> m_propertyIdLookup;
	QHash<QByteArray, QMetaType::Type> m_propertyTypeLookup;
	QHash<QByteArray, int> m_methodIdLookup;

	friend class BluemonkeyQObjectPrivate;
};

#endif
