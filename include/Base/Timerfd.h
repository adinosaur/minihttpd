//
// Date: 2015/11/03
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_TIMERFD_H
#define MINIHTTPD_TIMERFD_H

#include <sys/timerfd.h>
#include <string.h>
#include <time.h>

int Timerfd(int second)
{
    struct itimerspec howlong;

    int timerfd = ::timerfd_create(CLOCK_REALTIME, 0);
    
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = second;
    ::timerfd_settime(timerfd, 0, &howlong, NULL);

    return timerfd;
}
#endif
