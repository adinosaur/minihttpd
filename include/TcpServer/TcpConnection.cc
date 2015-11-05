
#include "TcpConnection.h"
#include "../Base/Logger.h"
#include <unistd.h>

using namespace std;

TcpConnection::TcpConnection(int connfd, const std::string& addr, int port):
    _connfd(connfd),
    _timeout(false),
    _curruct_channel(nullptr)
{
    Logger::instance(Logger::TRACE)->logging(__FILE__, __LINE__, 
        "NEW-CONNECTION:" + std::to_string(_connfd) + ":" + addr + ":" + std::to_string(port));
}

TcpConnection::~TcpConnection()
{
    close(_connfd);
    Logger::instance(Logger::TRACE)->logging(__FILE__, __LINE__, 
        "CONNECTION-CLOSE:" + std::to_string(_connfd));
}

void TcpConnection::set_timeout()
{
    _timeout = true;
}

bool TcpConnection::get_timeout()
{
    return _timeout;
}

void TcpConnection::set_channel(Channel* channel)
{
    _curruct_channel = channel;
}

Channel* TcpConnection::get_channel()
{
    return _curruct_channel;
}
