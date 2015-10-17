//
// Date: 2015/10/09
// Author: Dinosaur W.
//

#include "../src/include/Logger.h"
#include <iostream>
using namespace std;

void test()
{
    TRACE_LOG.logging("TRACK", "hello", 2);
    DEBUG_LOG.logging("DEBUG", "hello", 3);
    INFO_LOG.logging("INFO", 4 ,5);
    WARN_LOG.logging("WARN", 6, "hello");
    ERROR_LOG.logging("ERRRO");
}

int main()
{
    test();
    return 0;
}
