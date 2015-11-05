//
// Date: 2015/11/03
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_TCPCONNECTION_H
#define MINIHTTPD_TCPCONNECTION_H

#include <string>
class Channel;

class TcpConnection
{
    public:
        TcpConnection(int connfd, const std::string& addr, int port);
        ~TcpConnection();

        void set_timeout();
        bool get_timeout();

        void set_channel(Channel* channel);
        Channel* get_channel();
        
    private:
        int _connfd;
        bool _timeout;
        Channel* _curruct_channel;
};

#endif
