//
// Date: 2015/10/10
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_STRTIME_H
#define MINIHTTPD_STRTIME_H
#include <string>

class StrTime
{
    public:
        StrTime();
        std::string to_string(const char* format);
        
    private:
        
        time_t rawtime;
        struct tm * timeinfo;
};

#endif
