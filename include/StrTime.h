//
// Date: 2015/10/10
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_STRTIME_H
#define MINIHTTPD_STRTIME_H

#include <time.h>
#include <string>

class StrTime
{
    public:
        StrTime()
        {
            ::time(&rawtime);
            timeinfo = localtime(&rawtime);
        }
        
        std::string to_string(const char* format)
        {
            char buf[64];
            strftime(buf, sizeof(buf), format, timeinfo);
            return buf;
        }
        
    private:
        
        time_t rawtime;
        struct tm * timeinfo;
};

#endif
