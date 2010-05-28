// Logger.hpp
// 1.0

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 2 of the License.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <fstream>

using std::string;
using std::fstream;


#define MACRO_LOG_DEBUG(ARG) Logger::getInstance()->Debug(__FILE__, __LINE__, ARG );


/** Logger class */
class Logger
{
private:
    fstream logFile; //Our log file


    //The singleton to return
    static Logger* instanceLog;

public:
    //Constructor and destructor
    Logger();
    ~Logger();

    //Get an instance of our singleton
    static Logger* getInstance();

    //Free up our singleton.
    static void freeSingleton();

    //Log somthing...
    void Info(string str);
    void Info(string filename, int lignenumber, string str);
    
    void Debug(string str);
    void Debug(string filename, int lignenumber, string str);
    
    void Error(string str);
    void Error(string filename, int lignenumber, string str);
    
    void Warning(string str);
    void Warning(string filename, int lignenumber, string str);    
};

#endif //LOGGER_HPP
