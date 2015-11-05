//
// Date: 2015/11/03
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_TIMERFD_H
#define MINIHTTPD_TIMERFD_H

//
// 管理timerfd
//
class Timerfd
{
    public:
        Timerfd(int second);
        ~Timerfd();
        
        int fd();
    private:
        int timerfd_create(int second);
        
        int _timerfd;
};


#endif
