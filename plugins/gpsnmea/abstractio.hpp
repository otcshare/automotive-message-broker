#ifndef _ABSTRACTIO_H_
#define _ABSTRACTIO_H_

#include <string>

class AbstractIo
{
public:

	virtual bool open() = 0;
	virtual bool close() = 0;
	virtual std::string read() = 0;
	virtual void write(std::string data) = 0;

	virtual int fileDescriptor() = 0;
};

#endif
