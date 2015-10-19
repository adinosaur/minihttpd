//
// Date: 2015/10/19
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_PROTOCOL_H
#define MINIHTTPD_PROTOCOL_H

//
// EventLoop依赖的函数接口
// Http实现这些函数接口
//
class Protocol
{
    public:
        void accept_request();
        void handle_request();
        void send_response();
};

#endif
