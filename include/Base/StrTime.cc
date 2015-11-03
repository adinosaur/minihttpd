
#include "StrTime.h"
#include <time.h>
#include <string>
using namespace std;

StrTime::StrTime()
{
    ::time(&rawtime);
    timeinfo = localtime(&rawtime);
}

string StrTime::to_string(const char* format)
{
    char buf[64];
    strftime(buf, sizeof(buf), format, timeinfo);
    return buf;
}
