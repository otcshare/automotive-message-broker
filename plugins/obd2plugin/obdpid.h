#ifndef _OBDPID_H__H_H_
#define _OBDPID_H__H_H_

#include <vector>
#include <string>
#include <vehicleproperty.h>
#include <time.h>

class ObdPid
{
public:
	typedef std::vector<unsigned char> ByteArray;

	ObdPid(VehicleProperty::Property prop, std::string p, int i)
		:property(prop), pid(p), id(i), type(0x41)
	{

	}

	virtual ObdPid* create() = 0;

	virtual void parse(ByteArray replyVector) = 0;

	VehicleProperty::Property property;
	std::string pid;
	int id;
	int type;
	std::string value;
};

template <class T>
class CopyMe: public ObdPid
{
public:

	CopyMe(VehicleProperty::Property prop, std::string p, int i)
		:ObdPid(prop, p, i)
	{

	}

	ObdPid* create()
	{
		return new T();
	}
};


class VehicleSpeedPid: public CopyMe<VehicleSpeedPid>
{
public:

	VehicleSpeedPid()
		:CopyMe(VehicleProperty::VehicleSpeed, "010D1\r", 0x0D)
	{

	}

	void parse(ByteArray replyVector)
	{
		int mph = replyVector[2];
		value = boost::lexical_cast<std::string>(mph);
	}
};

class EngineSpeedPid: public CopyMe<EngineSpeedPid>
{
public:

	EngineSpeedPid()
		:CopyMe(VehicleProperty::VehicleSpeed,"010C1\r",0x0D)
	{

	}

	void parse(ByteArray replyVector)
	{
		double rpm = ((replyVector[2] << 8) + replyVector[3]) / 4.0;
		value = boost::lexical_cast<std::string>(rpm);
	}
};

class EngineCoolantPid: public CopyMe<EngineCoolantPid>
{
public:

	EngineCoolantPid()
		:CopyMe(VehicleProperty::VehicleSpeed,"01051\r",0x0D)
	{

	}

	void parse(ByteArray replyVector)
	{
		int temp = replyVector[2] - 40;
		value = boost::lexical_cast<std::string>(temp);
	}
};

class MassAirFlowPid: public CopyMe<MassAirFlowPid>
{
public:

	MassAirFlowPid()
		:CopyMe(VehicleProperty::VehicleSpeed,"01101\r",0x0D)
	{

	}

	virtual void parse(ByteArray replyVector)
	{
		maf = ((replyVector[2] << 8) + replyVector[3]) / 100.0;
		value = boost::lexical_cast<std::string>(maf);
	}

protected:
	double maf;
};


class FuelConsumptionPid: public MassAirFlowPid
{
public:
	FuelConsumptionPid()

	{

	}

	virtual void parse(ByteArray replyVector)
	{
		MassAirFlowPid::parse(replyVector);
		timespec t;
		clock_gettime(CLOCK_REALTIME, &t);

		double currentTime = t.tv_sec + t.tv_nsec / 1000000;

		double diffTime = currentTime - oldTime;
		oldTime = currentTime;

		double consumption = 1 / (14.75 * 6.26) * maf * diffTime/60;

		value = boost::lexical_cast<std::string>(consumption);
	}

private:

	static double oldTime;
};


class VinPid: public CopyMe<VinPid>
{
public:

	VinPid()
		:CopyMe(VehicleProperty::VIN,"0902\r",0x0D)
	{
		type = 0x49;
	}

	virtual void parse(ByteArray replyVector)
	{
		std::string vinstring;
		for (int j=0;j<replyVector.size();j++)
		{
			if(replyVector[j] == 0x49 && replyVector[j+1] == 0x02)
			{
				//We're at a reply header
				j+=3;
			}
			if (replyVector[j] != 0x00)
			{
				vinstring += (char)replyVector[j];
				//printf("VIN: %i %c\n",replyVector[j],replyVector[j]);
			}
		}

		value = vinstring;
	}

};

class WmiPid: public VinPid
{
public:

	WmiPid()
		:VinPid()
	{
		property = VehicleProperty::WMI;
	}

	virtual void parse(ByteArray replyVector)
	{
		VinPid::parse(replyVector);

		value = value.substr(0,3);
	}

};


#endif
