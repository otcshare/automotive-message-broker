#ifndef _BLE_H_
#define _BLE_H_

#include <QObject>
#include <QList>
#include <QString>
#include <QByteArray>
#include <QJSValue>

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QBluetoothUuid>

class QBluetoothDeviceDiscoveryAgent;
class QLowEnergyService;
class QLowEnergyCharacteristic;

class ServiceIdentifier : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString name READ getDeviceName)
	Q_PROPERTY(QString address READ getDeviceAddress)

public:
	ServiceIdentifier(const QString & s, const QString & r, const QString & t, QObject *parent = nullptr)
		:QObject(parent), serviceUuid(s), rxUuid(r), txUuid(t), service(nullptr), mController(nullptr),
	mAddyType(QLowEnergyController::RandomAddress){}

	QString getDeviceName() { return deviceName; }
	QString getDeviceAddress() { return deviceAddress; }
	void setRemoteAddressType(int t);

	QString serviceUuid;
	QString rxUuid;
	QString txUuid;
	QString deviceName;
	QString deviceAddress;

	QLowEnergyService * service;

public Q_SLOTS:
	QByteArray read();
	bool write(const QByteArray &data);
	void connectToDevice();
	void disconnectFromDevice();

Q_SIGNALS:
	void onMessage(const QByteArray & data);
	void connected();
	void disconnected();
	void stateChanged(int state);

private Q_SLOTS:
	void characteristicChanged(const QLowEnergyCharacteristic & characteristic, const QByteArray & newValue);
	void controllerConnected();
	void controllerServiceDiscovered(const QBluetoothUuid & uuid);
	void controllerStateChanged(QLowEnergyController::ControllerState state);
	void serviceStateChanged(QLowEnergyService::ServiceState s);

private:
	QLowEnergyController * mController;
	QLowEnergyController::RemoteAddressType mAddyType;
};

class Ble : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool scan READ scanning WRITE startScan NOTIFY scanningChanged)
	Q_PROPERTY(bool debug READ debug WRITE setDebug)
public:

	Ble(QObject* parent = nullptr);
	~Ble();
	QObject* device(const QString & address);
	bool debug();
	void setDebug(bool d);

public Q_SLOTS:

	void addService(const QString & serviceUuid, const QString & rxUuid, const QString & txUuid);

	void startScan(bool scan);

	bool scanning();

Q_SIGNALS:
	void scanningChanged();
	void leDeviceFound(QObject*);
	void error(int);

private Q_SLOTS:
	void errorHandle(QLowEnergyController::Error err);
	void deviceDiscovered(const QBluetoothDeviceInfo & device);

private:
	QList<ServiceIdentifier*> services;
	QBluetoothDeviceDiscoveryAgent* mDeviceDiscoveryAgent;
};


#endif

