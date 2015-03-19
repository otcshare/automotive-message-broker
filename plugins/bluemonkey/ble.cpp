#include "ble.h"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QLowEnergyCharacteristic>
#include <QtDebug>
#include <QCoreApplication>

bool mDebug = false;

#define DEBUG(msg) if(mDebug) qDebug() << msg;

extern "C" void create(std::map<std::string, std::string> config, std::map<std::string, QObject*> &exports, QString &js, QObject* parent)
{
	exports["ble"] = new Ble(parent);
}

Ble::Ble(QObject *parent)
	:QObject(parent)
{
	mDeviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

	connect(mDeviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &Ble::deviceDiscovered);
	connect(mDeviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &Ble::scanningChanged);
	connect(mDeviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &Ble::scanningChanged);
}

Ble::~Ble()
{
	DEBUG("Ble destroyed");
}

bool Ble::debug()
{
	return mDebug;
}

void Ble::setDebug(bool d)
{
	mDebug = d;
}

void Ble::addService(const QString &serviceUuid, const QString &rxUuid, const QString &txUuid)
{
	auto service = new ServiceIdentifier(serviceUuid, rxUuid, txUuid, this);

	connect(service, &ServiceIdentifier::destroyed, [this, &service]()
	{
		services.removeAll(service);
	});

	services.append(service);
}

void Ble::startScan(bool scan)
{
	DEBUG("Starting scan");

	if((scan && !mDeviceDiscoveryAgent->isActive()) || (!scan && mDeviceDiscoveryAgent->isActive()))
	{
		scan ? mDeviceDiscoveryAgent->start() : mDeviceDiscoveryAgent->stop();
		scanningChanged();
	}
}

void ServiceIdentifier::setRemoteAddressType(int t)
{
	mAddyType = QLowEnergyController::RemoteAddressType(t);
}

QByteArray ServiceIdentifier::read()
{
	QLowEnergyCharacteristic rx = service->characteristic(QBluetoothUuid(rxUuid));

	if(!rx.isValid())
	{
		DEBUG("rx characteristic is not valid");
		return "";
	}

	return rx.value();
}

bool Ble::scanning()
{
	return mDeviceDiscoveryAgent->isActive();
}

void Ble::errorHandle(QLowEnergyController::Error err)
{
	error((int)err);
}

void Ble::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
	if(device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration)
	{
		///We have a device.  Let's scan to see if it supports the service Uuid's we want
		ServiceIdentifier* service = nullptr;
		Q_FOREACH(QBluetoothUuid uuid, device.serviceUuids())
		{
			Q_FOREACH(auto s, services)
			{
				if(uuid.toString().contains(s->serviceUuid))
				{
					service = s;
					break;
				}
			}
			if(service)
				break;
		}

		if(!service)
		{
			DEBUG("This is not the device we are looking for.  No services found that we like (" << device.address() << ")");
			return;
		}

		service->deviceAddress = device.address().toString();
		service->deviceName = device.name();

		leDeviceFound(service);
		DEBUG("BLE device found: " << device.address().toString());
	}
}


bool ServiceIdentifier::write(const QByteArray &data)
{
	DEBUG("Writing: " << data << " To device: " << deviceName);
	QLowEnergyCharacteristic tx = service->characteristic(QBluetoothUuid(txUuid));

	if(!service || !tx.isValid())
	{
		DEBUG("Device is not really valid");
		DEBUG("is tx valid? " << tx.isValid());
		DEBUG("These are the characteristic uuids I see: ");
		Q_FOREACH(auto c, service->characteristics())
		{
			DEBUG(c.uuid().toString());
		}

		return false;
	}

	service->writeCharacteristic(service->characteristic(QBluetoothUuid(txUuid)), data, QLowEnergyService::WriteWithoutResponse);
	return true;
}

void ServiceIdentifier::connectToDevice()
{
	if(deviceAddress.isEmpty())
	{
		DEBUG("Device address not set.  aborting connectToDevice()");
		return;
	}
	if(!mController)
	{
		mController = new QLowEnergyController(QBluetoothAddress(deviceAddress), this);
		mController->setRemoteAddressType(mAddyType);
		connect(mController, &QLowEnergyController::connected, this, &ServiceIdentifier::controllerConnected);
		connect(mController, &QLowEnergyController::disconnected, this, &ServiceIdentifier::disconnected);
		connect(mController, &QLowEnergyController::serviceDiscovered, this, &ServiceIdentifier::controllerServiceDiscovered);
		connect(mController, &QLowEnergyController::stateChanged, this, &ServiceIdentifier::controllerStateChanged);
	}

	DEBUG("Connecting to device...");
	mController->connectToDevice();
}

void ServiceIdentifier::disconnectFromDevice()
{
	if(mController)
		mController->disconnectFromDevice();
}

void ServiceIdentifier::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
{
	if(!characteristic.uuid().toString().contains(rxUuid))
	{
		DEBUG("Some characteristic changed, but it wasn't our rx Uuid: " << characteristic.uuid().toString());
		DEBUG("Value: " << characteristic.value());
		return;
	}

	onMessage(newValue);
}

void ServiceIdentifier::controllerConnected()
{
	DEBUG("Device connected. Discovering services...");
	mController->discoverServices();
}

void ServiceIdentifier::controllerServiceDiscovered(const QBluetoothUuid &uuid)
{
	if(!uuid.toString().contains(serviceUuid))
	{
		DEBUG("This is not the service we are looking for: " << uuid.toString());
		return;
	}

	DEBUG("This is the service we are looking for!");

	service = mController->createServiceObject(uuid, this);

	DEBUG("service state: " << service->state());

	connect(service, &QLowEnergyService::stateChanged, this, &ServiceIdentifier::serviceStateChanged);
	connect(service, &QLowEnergyService::characteristicChanged, this, &ServiceIdentifier::characteristicChanged);
	service->discoverDetails();
}

void ServiceIdentifier::controllerStateChanged(QLowEnergyController::ControllerState state)
{
	stateChanged(state);
}

void ServiceIdentifier::serviceStateChanged(QLowEnergyService::ServiceState s)
{
	DEBUG("Service state changed to: " << s);
	if(s == QLowEnergyService::ServiceDiscovered)
		connected();
}

int main(int argc, char** argv)
{
	bool mDebug = true;

	DEBUG("Bluemonkey BLE Module"  << "Version: " << PROJECT_VERSION);

	QCoreApplication app(argc, argv);

	Ble ble;
	ble.setDebug(true);
	ble.addService("5faaf494-d4c6-483e-b592-d1a6ffd436c9", "5faaf495-d4c6-483e-b592-d1a6ffd436c9", "5faaf496-d4c6-483e-b592-d1a6ffd436c9");

	ble.startScan(true);

	return app.exec();
}

