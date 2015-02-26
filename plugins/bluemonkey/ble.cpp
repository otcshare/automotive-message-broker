#include "ble.h"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include <QLowEnergyCharacteristic>
#include <QtDebug>
#include <QCoreApplication>

#define DEBUG(msg) if(mDebug) qDebug() << msg;

extern "C" void create(std::map<std::string, std::string> config, std::map<std::string, QObject*> &exports, QString &js, QObject* parent)
{
	exports["ble"] = new Ble(parent);
}

Ble::Ble(QObject *parent)
	:QObject(parent), mAddyType(QLowEnergyController::RandomAddress), mDebug(false)
{
	mDeviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

	connect(mDeviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &Ble::deviceDiscovered);
	connect(mDeviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &Ble::scanningChanged);
	connect(mDeviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &Ble::scanningChanged);
}

QList<QObject *> Ble::devices()
{
	return devices(QString());
}

QList<QObject *> Ble::devices(const QString &serviceUuid)
{
	QList<QObject*> d;

	Q_FOREACH(auto s, services)
	{
		if(s->isValid && (serviceUuid.isEmpty() || serviceUuid == s->serviceUuid))
		{
			d.append(s);
		}
	}

	return d;
}

QObject *Ble::device(const QString &address)
{
	Q_FOREACH(auto s, services)
	{
		if(s->deviceAddress == address)
		{
			return s;
		}
	}

	return nullptr;
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
	if(scan && !mDeviceDiscoveryAgent->isActive())
		scanningChanged();
	else if(!scan && mDeviceDiscoveryAgent->isActive())
		scanningChanged();

	scan ? mDeviceDiscoveryAgent->start() : mDeviceDiscoveryAgent->stop();
}

void Ble::setRemoteAddressType(int t)
{
	mAddyType = QLowEnergyController::RemoteAddressType(t);
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
		leDeviceFound(device.name(), device.address().toString());
		DEBUG("BLE device found: " << device.address().toString());

		///We have a device.  Let's scan to see if it supports the service Uuid's we want
		auto control = new QLowEnergyController(device.address(), this);
		control->setRemoteAddressType(mAddyType);

		connect(control, &QLowEnergyController::stateChanged, [control, this](QLowEnergyController::ControllerState state)
		{
			DEBUG("Controller state changed for device: " << control->remoteAddress().toString() << state);

			if(state == QLowEnergyController::DiscoveredState)
			{
				devicesChanged();
			}
		});
		connect(control, &QLowEnergyController::disconnected, control, &QLowEnergyController::deleteLater); // if we disconnect, clean up.
		connect(control, &QLowEnergyController::connected, [this, &control]()
		{
			DEBUG("Device connected. Discovering services...");
			control->discoverServices();
		});
		connect(control, SIGNAL(error(QLowEnergyController::Error)), this, SLOT(errorHandle(QLowEnergyController::Error)));
		connect(control, &QLowEnergyController::serviceDiscovered, [&control, this, device](const QBluetoothUuid & uuid)
		{
			Q_FOREACH(ServiceIdentifier * service, services)
			{
				qDebug() << "checking if service uuid (" << uuid.toString() << ") matches the one we want (" << service->serviceUuid << ")";
				if(service->serviceUuid == uuid.toString())
				{
					service->service = control->createServiceObject(uuid, service);
					service->deviceName = device.name();
					service->deviceAddress = device.address().toString();

					if(!service->service)
					{
						qWarning() << "Could not get service object on device with address: " << control->remoteAddress().toString();
						return;
					}

					connect(service->service, &QLowEnergyService::stateChanged,[&service](QLowEnergyService::ServiceState s)
					{
						if(s == QLowEnergyService::ServiceDiscovered)
						{
							QLowEnergyCharacteristic rx = service->service->characteristic(QBluetoothUuid(service->rxUuid));

							if(!rx.isValid())
							{
								qWarning() << "rx characteristic not found in service.";
								qWarning() << "Characteristic: " << service->rxUuid;
								qWarning() << "Service: " << service->serviceUuid;
								service->isValid = false;
								return;
							}

							QLowEnergyCharacteristic tx = service->service->characteristic(QBluetoothUuid(service->txUuid));

							if(!tx.isValid())
							{
								qWarning() << "rx characteristic not found in service.";
								qWarning() << "Characteristic: " << service->rxUuid;
								qWarning() << "Service: " << service->serviceUuid;
								service->isValid = false;
								return;
							}

							service->isValid = true;
						}
					});
				}
			}
		});
		qDebug() << "Attempting to connect...";
		control->connectToDevice();
	}
}


void ServiceIdentifier::write(const QByteArray &data)
{
	{
		if(!service || !isValid)
			return;

		service->writeCharacteristic(service->characteristic(QBluetoothUuid(txUuid)), data, QLowEnergyService::WriteWithoutResponse);
	}
}

void ServiceIdentifier::characteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &newValue)
{
	if(characteristic.uuid().toString() != rxUuid)
		return;

	onMessage(newValue);
}

int main(int argc, char** argv)
{
	bool mDebug = true;

	DEBUG("Bluemonkey BLE Module"  << "Version: " << PROJECT_VERSION);

	QCoreApplication app(argc, argv);

	Ble ble;

	ble.startScan(true);

	return app.exec();
}

