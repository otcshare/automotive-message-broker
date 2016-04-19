#ifndef _SERIALPORT_H_
#define _SERIALPORT_H_

#include "abstractio.hpp"
#include "debugout.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdio.h>

class SerialPort: public AbstractIo
{
private:
	speed_t speed;

public:
	SerialPort()
		:fd(0), speed(B9600)
	{

	}

	SerialPort(int fileDesc)
		:fd(fileDesc)
	{
		speed = B9600;

		setDescriptor(fd);
	}

	SerialPort(std::string _tty)
		:tty(_tty), fd(0)
	{
		speed = B9600;
	}

	~SerialPort()
	{
		close();
	}

	int setSpeed(int newspeed)
	{
		int ret = 0;
		switch(newspeed)
		{
			case 2400:
				speed = B2400;
				break;
			case 4800:
				speed = B4800;
				break;
			case 9600:
				speed = B9600;
				break;
			case 19200:
				speed = B19200;
				break;
			case 38400:
				speed = B38400;
				break;
			default:
				ret = -EINVAL;
		}
		return ret;
	}

	bool open()
	{
		fd = ::open(tty.c_str(), O_RDWR, O_NOCTTY);

		return setupDevice();
	}

	bool isOpen()
	{
		return (fd > 0);
	}

	int fileDescriptor() { return fd; }

	bool close()
	{
		::close(fd);
	}

	std::string read()
	{
		char buff;
		std::string result;
		int bytesread = 0;
		while( bytesread = ::read(fd, &buff, 1) > 0 )
		{
			result += buff;
		}

		if(bytesread == -1)
			perror("Error while reading: ");

		return result;
	}

	void write(std::string data)
	{
		int written = ::write(fd,data.c_str(),data.length());
		if(written == -1)
		{
			DebugOut(DebugOut::Warning)<<"Unable to write ("<<fd<<")"<<endl;
			perror("write error: ");
		}
	}

	void setDescriptor(int d)
	{
		fd = d;
		//setupDevice();
	}

private: ///methods

	bool setupDevice()
	{
		if(fd == -1)
		{
			DebugOut()<<"Cannot open serial device."<<endl;
			return false;
		}

		struct termios oldtio;
		tcgetattr(fd,&oldtio);

		oldtio.c_cflag |= CS8 | CLOCAL | CREAD;

		oldtio.c_iflag |= IGNPAR;
		oldtio.c_iflag &= ~(ICRNL | IMAXBEL);


		oldtio.c_oflag &= ~OPOST;

		oldtio.c_lflag |= ECHOE | ECHOK | ECHOCTL | ECHOKE;
		oldtio.c_lflag &= ~(ECHO | ICANON | ISIG);

		//oldtio.c_cc[VEOL]     = '\r';

		cfsetispeed(&oldtio, speed);
		cfsetospeed(&oldtio, speed);

		tcflush(fd, TCIFLUSH);
		tcsetattr(fd, TCSANOW, &oldtio);

		fcntl(fd,F_SETFL,O_NONBLOCK);

		return true;
	}

private:
	int fd;
	std::string tty;

};


#endif
