#ifndef WSAERROR_H
#define WSAERROR_H

#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

class WSAError : public runtime_error
{
public:
    WSAError(const string& msg) ;
};

#endif //WSAERROR_H
