#ifndef ObdLinkMx_H_
#define ObdLinkMx_H_

#include <string>


class ObdLinkMx
{
public:
	enum Protocol{
		J1850PWM = 11,
		J1850VPW = 12,
		ISO9141 = 21,
		ISO9141_2 = 22,
		ISO14230 = 23,
		ISO14230_autoinit = 24,
		ISO14230_fastautoinit = 24,
		HSCAN_ISO11898_11bit_500kbps_varDLC = 31,
		HSCAN_ISO11898_29bit_500kbps_varDLC = 32,
		HSCAN_ISO15765_11bit_500kbps_DLC8 = 33,
		HSCAN_ISO15765_29bit_500kbps_DLC8 = 34,
		HSCAN_ISO15765_11bit_250kbps_DLC8 = 35,
		HSCAN_ISO15765_29bit_250kbps_DLC8 = 36,
		J1939_11bit = 41,
		J1939_29bit = 42,
		MSCAN_ISO11898_11bit_125kbps_varDLC = 51,
		MSCAN_ISO11898_29bit_125kbps_varDLC = 52,
		MSCAN_ISO15765_11bit_125kbps_DLC8 = 53,
		MSCAN_ISO15765_29bit_125kbps_DLC8 = 54,
		SWCAN_ISO11898_11bit_33kbps_varDLC = 61,
		SWCAN_ISO11898_29bit_33kbps_varDLC = 62,
		SWCAN_ISO15765_11bit_33kbps_DLC8 = 63,
		SWCAN_ISO15765_29bit_33kbps_DLC8 = 64
	};

	enum SWCanMode {
		Sleep = 0,
		HighSpeed,
		Wakeup,
		Normal
	};

	ObdLinkMx();

	void connect();
	void disconnect();

	void setProtocol(Protocol protocol);

	Protocol protocol();

	std::string protocolString();

	void setSwCanMode(SWCanMode mode);
	
	void monitor();
};


#endif //ObdLinkMx_H_
