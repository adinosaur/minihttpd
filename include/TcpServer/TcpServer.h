//
// Date: 2015/11/01
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_TCPSERVER_H
#define MINIHTTPD_TCPSERVER_H

#include "../EventLoop/EventLoop.h"
#include "../Base/Logger.h"
#include "../Base/Timerfd.h"
#include "TcpConnection.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/timerfd.h>

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
            std::thread log_thread(Logger::thread_func);
            log_thread.detach();
            
            make_threads();
            
            while (1)
                pause();
        }
        
        //
        // 处理请求返回响应
        //
        void handle(int connfd)
        {
            Protocol protocol(connfd);
            protocol.accept_request();
            protocol.handle_request();
            protocol.send_response();
        }
        
        //
        // 线程池
        //
        void make_threads()
        {
            // 成员函数指针
            auto fcn = std::mem_fn(&EventLoop<IOMulti>::loop);
            
            for(int i = 0; i != _threads; ++i)
            {
                auto eventloop = new EventLoop<IOMulti>(_sockfd);
                listen_channel(_sockfd, eventloop);
                _eventloops.push_back(eventloop);
                std::thread loop_thread(fcn, eventloop);
                loop_thread.detach();
            }
        }
        
        //
        // 封装::accept
        //
        int accept(int fd, struct sockaddr* addr, socklen_t* len)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            int connfd = ::accept(fd, addr, len);
            if (connfd < 0)
            {
                Logger::instance(Logger::ERROR)->logging(__FILE__, __LINE__, "ACCEPT-ERROR");
                exit(1);
            }
            return connfd;
        }
        
        //
        // 工作Channel
        //
        void working_channel(int connfd, TcpConnection* tcpConnection, EventLoop<IOMulti>* eventLoop)
        {
            Channel* channel = new Channel(connfd);
            channel->set_enable_reading();
            channel->set_read_callback([=]()
            {
                handle(channel->fd());
                
                if (tcpConnection->get_timeout())
                {
                    eventLoop->del_channel(channel);
                    delete channel;
                    delete tcpConnection;
                }
            });
            eventLoop->add_channel(channel);
            tcpConnection->set_channel(channel);
        }
        
        //
        // 计时器Channel
        //
        void timer_channel(int timerfd, TcpConnection* tcpConnection, EventLoop<IOMulti>* eventLoop)
        {
            Channel* channel = new Channel(timerfd);
            channel->set_enable_reading();
            channel->set_read_callback([=]()
            {
                tcpConnection->set_timeout();
                
                eventLoop->del_channel(channel);
                close(channel->fd());
                delete channel;
            });
            eventLoop->add_channel(channel);
        }
        
        //
        // 监听Channel
        //
        void listen_channel(int listenfd, EventLoop<IOMulti>* eventLoop)
        {
            Channel* channel = new Channel(listenfd);
            channel->set_enable_reading();
            channel->set_read_callback([=]()
            {
                char client_addr[32];
                struct sockaddr_in cliaddr;
                socklen_t length;
                
                // accept
                int connfd = this->accept(channel->fd(), (struct sockaddr*)(&cliaddr), &length);
                inet_ntop(AF_INET, &cliaddr.sin_addr, client_addr, sizeof(client_addr));

                // TcpConnection管理tcp连接
                TcpConnection* tcpConnection = new TcpConnection(connfd, client_addr, ntohs(cliaddr.sin_port));
                
                // timer
                int timerfd = Timerfd(30);
                timer_channel(timerfd, tcpConnection, eventLoop);
                working_channel(connfd, tcpConnection, eventLoop);
            });
            eventLoop->add_channel(channel);
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
                Logger::instance(Logger::ERROR)->logging(__FILE__, __LINE__, "SOCKET-CREATE-ERROR");
                exit(1);
            }
            
            int flag = 1;
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0)
            {
                Logger::instance(Logger::ERROR)->logging(__FILE__, __LINE__, "SET-SOCKET-OPT: SO_REUSEADDR");
                exit(1);
            }
            
            // bind
            bzero(&seraddr, sizeof(seraddr));
            seraddr.sin_family = AF_INET;
            seraddr.sin_port = htons(port);
            seraddr.sin_addr.s_addr = htonl(INADDR_ANY);
            if (bind(sockfd, (struct sockaddr*)(&seraddr), sizeof(seraddr)) < 0)
            {
                Logger::instance(Logger::ERROR)->logging(__FILE__, __LINE__, "BIND-ERROR");
                exit(1);
            }
            
            // listen
            if (listen(sockfd, 5) < 0)
            {
                Logger::instance(Logger::ERROR)->logging(__FILE__, __LINE__, "LISTEN-ERROR");
                exit(1);
            }
            
            return sockfd;
        }
    private:
        uint32_t _port;
        int _threads;
        int _sockfd;
        std::mutex _mutex;
        std::vector<EventLoop<IOMulti>*> _eventloops;
};

#endif
