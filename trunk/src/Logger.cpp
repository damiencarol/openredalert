
#include "Logger.hpp"

#include <string>
#include <fstream>

using std::string;
using std::fstream;
using std::endl;

//Initalize our static varaibles
Logger* Logger::instanceLog = NULL;;

//Log File constructor
Logger::Logger()
{
    logFile.open("log.txt", fstream::out); //Open log file for writing
}

//In our de-contructor, we close our log file
Logger::~Logger()
{
    logFile.close(); //Close our log file
}

//Log some text to our log file
void Logger::Info(string str)
{
    logFile << "[INFO]" << str << endl;
}

//Log some text to our log file
void Logger::Info(string filename, int linenumber, string str)
{
    logFile << "[INFO] " << filename << " (" << linenumber << ") :" << str << endl;
}

//Log some text to our log file
void Logger::Error(string str)
{
    logFile << "[ERROR]" << str << endl;
}

//Log some text to our log file
void Logger::Error(string filename, int linenumber, string str)
{
    logFile << "[Error] " << filename << " (" << linenumber << ") :" << str << endl;
}

//Log some text to our log file
void Logger::Warning(string str)
{
    logFile << "[WARNING]" << str << endl;
}

//Log some text to our log file
void Logger::Warning(string filename, int linenumber, string str)
{
    logFile << "[WARNING] " << filename << " (" << linenumber << ") :" << str << endl;
}

//Log some text to our log file
void Logger::Debug(string str)
{
    logFile << "[DEBUG]" << str << endl;
}

//Log some text to our log file
void Logger::Debug(string filename, int linenumber, string str)
{
    logFile << "[DEBUG] " << filename << " (" << linenumber << ") :" << str << endl;
}

//Our singleton implementation returns a pointer to our log class
Logger* Logger::getInstance()
{
    if (NULL == instanceLog)
    {
        //Get an instance of our logger
        instanceLog = new Logger();
    }

    return instanceLog;
}

//Cleanup after ourselves
void Logger::freeSingleton()
{
    if( instanceLog != NULL )
    {
        delete instanceLog;
        instanceLog = NULL;
    }
}
