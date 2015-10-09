//
// Date: 2015/10/08
// Author: Dinosaur W.
//

#include "include/EventLoop.h"

int main()
{
    EventLoop loop(8080);
    loop.loop();
    return 0;
}
