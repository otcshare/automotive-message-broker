#ifndef _SERIALPORT_H_
#define _SERIALPORT_H_

#include "abstractio.hpp"
#include "debugout.h"
#include "thread.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <queue>

class SerialPort: public AbstractIo, public CUtil::Thread
{
private:
	speed_t speed;
	char    terminator;
	pthread_mutex_t datamutex;
	std::queue<std::string> lines;

public:
	SerialPort()
		:fd(0), speed(B9600), terminator('\n')
	{
	}

	SerialPort(int fileDesc)
		:fd(fileDesc), speed(B9600), terminator('\n')
	{
		setDescriptor(fd);
	}

	SerialPort(std::string _tty)
		:tty(_tty), fd(0), speed(B9600), terminator('\n')
	{
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

	void setTerminator( char newterm )
	{
		terminator = newterm;
	}

	bool open()
	{
		bool isOpen;
		fd = ::open(tty.c_str(), O_RDWR, O_NOCTTY);
		isOpen = setupDevice();
		if(isOpen)
		{
			start();
		}
		return isOpen;
	}

	bool isOpen()
	{
		return (fd > 0);
	}

	int fileDescriptor() { return fd; }

	bool close()
	{
		stop();
		::close(fd);
	}

	virtual void run() 
	{
		char buff;
		int bytesread;
		std::string line;
		while(isRunnable(0))
		{ 
			bytesread = ::read(fd, &buff, 1);
			if(bytesread>0)
			{
				line += buff;
				if(buff==terminator)
				{
					pthread_mutex_lock(&datamutex);
					lines.push(line);
					pthread_mutex_unlock(&datamutex);
					line = "";
				}
			}
		}
	}

	std::string read()
	{
		std::string result;
		pthread_mutex_lock(&datamutex);        
		if(!lines.empty())
		{
			result = lines.front();
			lines.pop();
		}
		pthread_mutex_unlock(&datamutex);
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
        memset (&oldtio, 0, sizeof(oldtio));
		tcgetattr(fd,&oldtio);

		oldtio.c_cflag |= CS8 | CLOCAL | CREAD;

		oldtio.c_iflag |= IGNPAR;
		oldtio.c_iflag &= ~(ICRNL | IMAXBEL);


		oldtio.c_oflag &= ~OPOST;

		oldtio.c_lflag |= ECHOE | ECHOK | ECHOCTL | ECHOKE;
		oldtio.c_lflag &= ~(ECHO | ICANON | ISIG);

		//oldtio.c_cc[VEOL]     = '\r';
		oldtio.c_cc[VMIN] = 1; // 1 for Blocking
		oldtio.c_cc[VTIME] = 5; // 0.5 seconds read timeout

		cfsetispeed(&oldtio, speed);
		cfsetospeed(&oldtio, speed);

		tcflush(fd, TCIFLUSH);
		tcsetattr(fd, TCSANOW, &oldtio);

		return true;
	}

private:
	int fd;
	std::string tty;

};


#endif
