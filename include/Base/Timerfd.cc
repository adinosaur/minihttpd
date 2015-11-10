#include "../Base/Logger.h"
#include "Timerfd.h"
#include <sys/timerfd.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int Timerfd_create(int second)
{
    struct itimerspec howlong;
    int timerfd = ::timerfd_create(CLOCK_REALTIME, 0);
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = second;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    return timerfd;
}
