//
// Date: 2015/10/08
// Author: Dinosaur W.
//

#include "../include/EventLoop/EventLoop.h"
#include "../include/Http/Http.h"

int main()
{
    EventLoop<Http> httpServer(8080);
    httpServer.loop();
    return 0;
}
