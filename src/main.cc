//
// Date: 2015/10/08
// Author: Dinosaur W.
//

#include "../include/EventLoop/EventLoop.h"
#include "../include/EventLoop/IOMultiplexing.h"
#include "../include/Http/Http.h"

int main()
{
    //EventLoop<Http, Epoll> httpServer(8080);
    EventLoop<Http> httpServer(8080);
    httpServer.loop();
    return 0;
}
