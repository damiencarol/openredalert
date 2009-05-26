// Logger.cpp
//
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

#include <iostream> // for use of 'cout'

#include <cstdarg> // for use fct like 'printf'

#include <cstdio>
#include <cstdlib>

#include "include/Logger.h"
#include "video/MessagePool.h"
#include "vfs/vfs.h"
#include "vfs/VFile.h"

using std::cout;

namespace pc {
	extern MessagePool * msg;
}

Logger::Logger(const char *logname, int threshold)
{
    logfile = VFSUtils::VFS_Open(logname, "w");
#if !defined _WIN32
    if (logfile == 0) {
        printf("Unable to open logfile \"%s\" for writing, using stdout instead\n", logname);
    }
#endif
    indentsteps = 0;
    indentString[0] = '\0';
    rendergamemsg = false;
    this->threshold = threshold;
}

/**
 * Close the logfile at exit
 */
Logger::~Logger(){
	VFSUtils::VFS_Close(logfile);
}

void Logger::indent(){
    if( indentsteps > 60 ) {
        indentsteps = 62;
    } else {
        indentString[indentsteps] = ' ';
        indentString[indentsteps+1] = ' ';
        indentsteps+=2;
    }
    indentString[indentsteps] = '\0';
}

void Logger::unindent(){
    if( indentsteps < 3 ) {
        indentsteps = 0;
    } else {
        indentsteps -= 2;
    }
    indentString[indentsteps] = '\0';
}

void Logger::error(const char *txt, ...)
{
    // Parse parameters
    va_list ap, aq;
    va_start(ap, txt);
    va_start(aq, txt);
    
    // Log to file
    if (logfile != NULL) {
        logfile->vfs_printf("%sERROR: ", indentString);
        logfile->vfs_vprintf(txt, aq);
        logfile->flush();
    }

    // Log to the console
    printf("%sERROR: ", indentString);
    vprintf(txt, ap);
    cout.flush();


    // End of parsing
    va_end(aq);
    va_end(ap);
}

void Logger::debug(const char *txt, ...)
{
    va_list ap, aq;
    va_start(ap, txt);
    va_start(aq, txt);
    
    // Log to file
    if (logfile != 0) {
        logfile->vfs_printf("%sDEBUG: ", indentString);
        logfile->vfs_vprintf(txt, aq);
        logfile->flush();
    }
    
#if !defined _WIN32
    printf("%sDEBUG: ", indentString);
    vprintf(txt, ap);
    cout.flush();
#endif

    va_end(aq);
    va_end(ap);
}

void Logger::warning(const char *txt, ...)
{
    va_list ap, aq;
    va_start(ap, txt);
    va_start(aq, txt);
    if (logfile != NULL) {
        logfile->vfs_printf("%sWARNING: ", indentString);
        logfile->vfs_vprintf(txt, aq);
        logfile->flush();
    }
#if !defined _WIN32
    if (threshold > 0) {
        printf("%sWARNING: ", indentString);
        vprintf(txt,ap);
        cout.flush();
    }
#endif
    va_end(aq);
    va_end(ap);
}

void Logger::note(const char *txt, ...)
{
    va_list ap, aq;
    va_start(ap, txt);
    va_start(aq, txt);
    if (logfile != NULL) {
        logfile->vfs_printf("%s", indentString);
        logfile->vfs_vprintf(txt, aq);
        logfile->flush();
    }
#if !defined _WIN32
    if (threshold > 1) {
        printf("%s", indentString);
        vprintf(txt,ap);
        cout.flush();
    }
#endif
    va_end(aq);
    va_end(ap);
}

void Logger::gameMsg(const char *txt, ...)
{
    va_list ap;
    va_start(ap, txt);
    
    if (logfile != 0) 
    {
        logfile->vfs_printf("%sGame: ", indentString);
        logfile->vfs_vprintf(txt, ap);
        logfile->vfs_printf("\n");
        logfile->flush();
    }
    va_end(ap);

    if (rendergamemsg && (pc::msg != 0))
    {
        char msgstring[64];
        va_start(ap, txt);
        #ifdef _MSC_VER
        _vsnprintf(msgstring, 64, txt, ap);
        #else
        vsnprintf(msgstring, 64, txt, ap);
        #endif
        va_end(ap);
        pc::msg->postMessage(msgstring);
    } 
    else
    {
#if !defined _WIN32
        va_start(ap, txt);
        printf("%s",indentString);
        vprintf(txt,ap);
        va_end(ap);
        printf("\n");
        cout.flush();
#endif
    }
}

/**
 * Set if we must render message in game
 */
void Logger::renderGameMsg(bool r)
{
    rendergamemsg = r;
}

