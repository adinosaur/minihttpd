#include "../Base/Logger.h"
#include "Timerfd.h"
#include <sys/timerfd.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

Timerfd::Timerfd(int second):
    _timerfd(timerfd_create(second))
{
    Logger::instance(Logger::TRACE)->logging(__FILE__, __LINE__, 
        "TIMERFD-OPEN:" + std::to_string(_timerfd));
}

Timerfd::~Timerfd()
{
    close(_timerfd);
    Logger::instance(Logger::TRACE)->logging(__FILE__, __LINE__, 
        "TIMERFD-CLOSE:" + std::to_string(_timerfd));
}

int Timerfd::fd()
{
    return _timerfd;
}

int Timerfd::timerfd_create(int second)
{
    struct itimerspec howlong;
    int timerfd = ::timerfd_create(CLOCK_REALTIME, 0);
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = second;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    return timerfd;
}
