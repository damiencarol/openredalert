#ifndef LOGGER_H
#define LOGGER_H

class VFile;

class Logger {
public:
	Logger(const char *logname, int threshold);
	~Logger();

	void error(const char *txt, ...);
	void debug(const char *txt, ...);
	void warning(const char *txt, ...);
	void note(const char *txt, ...);
	void gameMsg(const char *txt, ...);
	void renderGameMsg(bool r);

	void indent();
	void unindent();
private:
	VFile * logfile;
	int indentsteps;
	int threshold;
	char indentString[64];
	bool rendergamemsg;
};

#endif

