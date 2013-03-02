#ifndef IMAINLOOP_H_
#define IMAINLOOP_H_



class IMainLoop {
public:

	IMainLoop(int argc, char **argv)
		:mArgc(argc),mArgv(argv)
	{

	}

	virtual ~IMainLoop() { }

	virtual int exec() = 0;

private:
	int mArgc;
	char** mArgv;
};


#endif
