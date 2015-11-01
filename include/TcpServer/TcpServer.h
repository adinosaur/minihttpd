//
// Date: 2015/11/01
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_TCPSERVER_H
#define MINIHTTPD_TCPSERVER_H

#include "../EventLoop/EventLoop.h"

#include <unistd.h>

#include <functional>
#include <vector>
#include <thread>
#include <mutex>

class Epoll;

template <typename Protocol, typename IOMulti = Epoll>
class TcpServer
{
    public:
        TcpServer(uint32_t port, int threads):
            _port(port),
            _threads(threads),
            _sockfd(setup(_port))
        {
        }
        
        ~TcpServer()
        {
            for (auto eventloop : _eventloops)
                delete eventloop;
            close(_sockfd);
        }
        
        // 禁止copy
        TcpServer(const TcpServer&) = delete;
        TcpServer& operator=(const TcpServer&) = delete;
        
        //
        // 运行
        //
        void run()
        {
            make_threads();
            
            while (1)
                pause();
        }
        
        //
        // 线程池
        //
        void make_threads()
        {
            // 成员函数指针
            auto fcn = std::mem_fn(&EventLoop<Protocol, IOMulti>::loop);
            
            for(int i = 0; i != _threads; ++i)
            {
                auto eventloop = new EventLoop<Protocol, IOMulti>(_sockfd, &_mutex);
                _eventloops.push_back(eventloop);
                std::thread loop_thread(fcn, eventloop);
                loop_thread.detach();
            }
        }
        
        //
        // 初始化，执行:
        // 1. 创建socket. 2. 将socket绑定IP. 3. 监听
        // 返回监听的文件描述符
        //
        int setup(uint32_t port)
        {
            struct sockaddr_in seraddr;
            
            // create socket
            int sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0)
            {
                ERROR_LOG.logging(__FILE__, __LINE__, "SOCKET-CREATE-ERROR");
                exit(1);
            }
            
            int flag = 1;
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0)
            {
                ERROR_LOG.logging(__FILE__, __LINE__, "SET-SOCKET-OPT", "SO_REUSEADDR");
                exit(1);
            }
            
            // bind
            bzero(&seraddr, sizeof(seraddr));
            seraddr.sin_family = AF_INET;
            seraddr.sin_port = htons(port);
            seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
            if (bind(sockfd, (struct sockaddr*)(&seraddr), sizeof(seraddr)) < 0)
            {
                ERROR_LOG.logging(__FILE__, __LINE__, "BIND-ERROR");
                exit(1);
            }
            
            // listen
            if (listen(sockfd, 5) < 0)
            {
                ERROR_LOG.logging(__FILE__, __LINE__, "LISTEN-ERROR");
                exit(1);
            }
            
            return sockfd;
        }
    private:
        uint32_t _port;
        int _threads;
        int _sockfd;
        std::mutex _mutex;
        std::vector<EventLoop<Protocol, IOMulti>*> _eventloops;
};

#endif
