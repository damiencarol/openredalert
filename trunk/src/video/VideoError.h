#ifndef VIDEOERROR_H
#define VIDEOERROR_H

#include <string>
#include <stdexcept>

using std::string;
using std::runtime_error;

class VideoError : public runtime_error {
public:    
    VideoError(const string& msg);
};

#endif //VIDEOERROR_H
