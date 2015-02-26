#ifndef _BLE_H_
#define _BLE_H_

#include <QObject>
#include <QList>
#include <QString>
#include <QByteArray>
#include <QJSValue>

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

class QBluetoothDeviceDiscoveryAgent;
class QLowEnergyService;
class QLowEnergyCharacteristic;

class ServiceIdentifier : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString deviceName READ getDeviceName)
	Q_PROPERTY(QString deviceName READ getDeviceAddress)

public:
	ServiceIdentifier(const QString & s, const QString & r, const QString & t, QObject *parent = nullptr)
		:QObject(parent), serviceUuid(s), rxUuid(r), txUuid(t), service(nullptr), isValid(false) {}

	QString getDeviceName() { return deviceName; }
	QString getDeviceAddress() { return deviceAddress; }

	QString serviceUuid;
	QString rxUuid;
	QString txUuid;
	QString deviceName;
	QString deviceAddress;

	QLowEnergyService * service;

	bool isValid;

public Q_SLOTS:
	void write(const QByteArray & data);

Q_SIGNALS:
	void onMessage(const QByteArray & data);

private Q_SLOTS:
	void characteristicChanged(const QLowEnergyCharacteristic & characteristic, const QByteArray & newValue);
};

class Ble : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool scan READ scanning WRITE startScan NOTIFY scanningChanged)
	Q_PROPERTY(QList<QObject*> devices READ devices NOTIFY devicesChanged)
	Q_PROPERTY(bool debug READ debug WRITE setDebug)
public:

	Ble(QObject* parent = nullptr);

	QList<QObject*> devices();
	QList<QObject*> devices(const QString & serviceUuid);
	QObject* device(const QString & address);
	bool debug() { return mDebug; }
	void setDebug(bool d) { mDebug = d; }

public Q_SLOTS:

	void addService(const QString & serviceUuid, const QString & rxUuid, const QString & txUuid);

	void startScan(bool scan);
	void setRemoteAddressType(int t);

	bool scanning();

Q_SIGNALS:
	void scanningChanged();
	void devicesChanged();
	void leDeviceFound(QString name, QString address);
	void error(int);

private Q_SLOTS:
	void errorHandle(QLowEnergyController::Error err);
	void deviceDiscovered(const QBluetoothDeviceInfo & device);

private:
	bool mDebug;
	QLowEnergyController::RemoteAddressType mAddyType;
	QList<ServiceIdentifier*> services;
	QBluetoothDeviceDiscoveryAgent* mDeviceDiscoveryAgent;
};


#endif

