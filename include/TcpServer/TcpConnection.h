//
// Date: 2015/11/03
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_TCPCONNECTION_H
#define MINIHTTPD_TCPCONNECTION_H

#include "../Base/Logger.h"
#include <unistd.h>
#include <string>

class Channel;

class TcpConnection
{
    public:
        TcpConnection(int connfd, const std::string& addr, int port):
            _connfd(connfd),
            _timeout(false),
            _curruct_channel(nullptr)
        {
            Logger::instance(Logger::TRACE)->logging(__FILE__, __LINE__, 
                "NEW-CONNECTION:" + std::to_string(_connfd) + ":" + addr + ":" + std::to_string(port));
        }
        
        ~TcpConnection()
        {
            close(_connfd);
            Logger::instance(Logger::TRACE)->logging(__FILE__, __LINE__, 
                "CONNECTION-CLOSE:" + std::to_string(_connfd));
        }

        void set_timeout()
        {
            _timeout = true;
        }
        
        bool get_timeout()
        {
            return _timeout;
        }

        void set_channel(Channel* channel)
        {
            _curruct_channel = channel;
        }
        
        Channel* get_channel()
        {
            return _curruct_channel;
        }
        
    private:
        int _connfd;
        bool _timeout;
        Channel* _curruct_channel;
};

#endif
