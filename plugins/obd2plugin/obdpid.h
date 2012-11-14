#ifndef _OBDPID_H__H_H_
#define _OBDPID_H__H_H_

#include <vector>
#include <string>
#include <vehicleproperty.h>
#include "obdlib.h"
#include <time.h>

class ObdPid
{
public:
	typedef std::vector<unsigned char> ByteArray;

	ObdPid(VehicleProperty::Property prop, std::string p, int i)
		:property(prop), pid(p), id(i), type(0x41)
	{

	}
	static ByteArray cleanup(ByteArray replyVector)
	{
		ByteArray tmp;
		for (int i=0;i<replyVector.size();i++)
		{
			if ((replyVector[i] != 0x20) && (replyVector[i] != '\r') && (replyVector[i] != '\n'))
			{
				tmp.push_back(replyVector[i]);
			}
		}
		return tmp;
	}
	static ByteArray compress(ByteArray replyVector)
	{
		ByteArray tmp;
		for (int i=0;i<replyVector.size();i++)
		{
			tmp.push_back(obdLib::byteArrayToByte(replyVector[i],replyVector[i+1]));
			i++;
		}
		return tmp;
	}
	virtual ObdPid* create() = 0;

	virtual bool tryParse(ByteArray replyVector) = 0;

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
	bool tryParse(ByteArray replyVector)
	{
		ByteArray tmp = compress(cleanup(replyVector));
		for (int i=0;i<tmp.size();i++)
		{
		  printf("%i ",tmp[i]);
		}
		printf("\n");
		if (tmp[1] != 0x0D)
		{
			return false;
		}
		int mph = tmp[2];
		value = boost::lexical_cast<std::string>(mph);
		return true;
	}
};

class EngineSpeedPid: public CopyMe<EngineSpeedPid>
{
public:

	EngineSpeedPid()
		:CopyMe(VehicleProperty::VehicleSpeed,"010C1\r",0x0D)
	{

	}
	bool tryParse(ByteArray replyVector)
	{
		ByteArray tmp = compress(cleanup(replyVector));
		if (tmp[1] != 0x0C)
		{
			return false;
		}
		double rpm = ((tmp[2] << 8) + tmp[3]) / 4.0;
		value = boost::lexical_cast<std::string>(rpm);
		return false;
	}
};

class EngineCoolantPid: public CopyMe<EngineCoolantPid>
{
public:

	EngineCoolantPid()
		:CopyMe(VehicleProperty::VehicleSpeed,"01051\r",0x0D)
	{

	}
	bool tryParse(ByteArray replyVector)
	{
		ByteArray tmp = compress(cleanup(replyVector));
		if (tmp[1] != 0x05)
		{
			return false;
		}
		int temp = tmp[2] - 40;
		value = boost::lexical_cast<std::string>(temp);
		return false;
	}
};

class MassAirFlowPid: public CopyMe<MassAirFlowPid>
{
public:

	MassAirFlowPid()
		:CopyMe(VehicleProperty::VehicleSpeed,"01101\r",0x0D)
	{

	}
	bool tryParse(ByteArray replyVector)
	{
		ByteArray tmp = compress(cleanup(replyVector));
		if (tmp[1] != 0x10)
		{
			return false;
		}
		maf = ((tmp[2] << 8) + tmp[3]) / 100.0;
		value = boost::lexical_cast<std::string>(maf);
		return false;
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
	bool tryParse(ByteArray replyVector)
	{
		MassAirFlowPid::tryParse(replyVector);
		timespec t;
		clock_gettime(CLOCK_REALTIME, &t);

		double currentTime = t.tv_sec + t.tv_nsec / 1000000;

		double diffTime = currentTime - oldTime;
		oldTime = currentTime;

		double consumption = 1 / (14.75 * 6.26) * maf * diffTime/60;

		value = boost::lexical_cast<std::string>(consumption);
		return false;
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
	bool tryParse(ByteArray replyVector)
	{
		std::string vinstring;
		ByteArray tmp = compress(cleanup(replyVector));
		if (tmp[0] != 0x49 && tmp[1] != 0x02)
		{
			return false;
		}
		for (int j=0;j<tmp.size();j++)
		{
			if(tmp[j] == 0x49 && tmp[j+1] == 0x02)
			{
				//We're at a reply header
				j+=3;
			}
			if (tmp[j] != 0x00)
			{
				vinstring += (char)tmp[j];
				//printf("VIN: %i %c\n",replyVector[j],replyVector[j]);
			}
		}

		value = vinstring;
		return false;
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
	bool tryParse(ByteArray replyVector)
	{
		VinPid::tryParse(replyVector);

		value = value.substr(0,3);
		return false;
	}

};


#endif
