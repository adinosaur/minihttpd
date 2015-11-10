//
// Date: 2015/11/01
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_TCPSERVER_H
#define MINIHTTPD_TCPSERVER_H

#include "../EventLoop/EventLoop.h"
#include "../Base/Logger.h"
#include "../Base/Timerfd.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <list>
#include <map>

class Epoll;

template <typename Protocol, typename IOMulti = Epoll>
class TcpServer
{
    public:
        typedef std::function<void()> CallBack;
        
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
        
    private:
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
        
        //
        // 处理请求返回响应
        //
        void handle(int connfd)
        {
            Logger::instance(Logger::INFO)->logging(__FILE__, __LINE__, "NEW-REQUEST:" + std::to_string(connfd));
            Protocol protocol(connfd);
            protocol.handle();
        }
        
        //
        // 线程池
        //
        void make_threads()
        {
            // 成员函数指针
            auto fcn = std::mem_fn(&TcpServer<Protocol, IOMulti>::loop);
            
            for(int i = 0; i != _threads; ++i)
            {
                auto eventloop = new EventLoop<IOMulti>(_sockfd);
                _closing_list[eventloop] = std::list<Channel*>();
                
                listen_channel(_sockfd, eventloop);
                _eventloops.push_back(eventloop);
                
                std::thread loop_thread(fcn, this, eventloop);
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
        // 监听Channel
        //
        void listen_channel(int listenfd, EventLoop<IOMulti>* eventloop)
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
                Logger::instance(Logger::INFO)->logging(__FILE__, __LINE__, 
                    "NEW-CONNECTION:" + std::to_string(connfd) + ":" + client_addr + ":" + std::to_string(cliaddr.sin_port));

                // 计时器fd
                int timerfd = Timerfd_create(10);
                
                // TCP连接Channel
                Channel* connect_channel = new Channel(connfd);
                connect_channel->set_enable_reading();
                connect_channel->set_read_callback([=]()
                {
                    handle(connfd);
                });
                eventloop->add_channel(connect_channel);
                
                // 计时器Channel
                Channel* timer_channel = new Channel(timerfd);
                timer_channel->set_enable_reading();
                timer_channel->set_read_callback([=]()
                {
                    _closing_list[eventloop].push_back(connect_channel);
                    _closing_list[eventloop].push_back(timer_channel);
                    
                    Logger::instance(Logger::INFO)->logging(__FILE__, __LINE__, 
                        "CONNECTION-CLOSE:" + std::to_string(connfd));
                });
                eventloop->add_channel(timer_channel);

            });
            eventloop->add_channel(channel);
        }
        
        //
        // 事件循环
        //
        void loop(EventLoop<IOMulti>* eventloop)
        {
            while (1)
            {
                eventloop->loop();
                clean_channel(eventloop);
            }
        }
        
        //
        // 删除TimeOut的tcp连接
        //
        void clean_channel(EventLoop<IOMulti>* eventloop)
        {
            for (auto it = _closing_list[eventloop].begin();
                it != _closing_list[eventloop].end(); )
            {
                Channel* channel = (*it);
                it = _closing_list[eventloop].erase(it);
                
                // 注销事件循环
                eventloop->del_channel(channel);
                
                // 关闭文件描述符
                close(channel->fd());
                
                // 回收Channel对象资源
                delete channel;
            }
        }
        
        uint32_t _port;
        int _threads;
        int _sockfd;
        std::mutex _mutex;
        std::vector<EventLoop<IOMulti>*> _eventloops;
        std::map<EventLoop<IOMulti>*, std::list<Channel*>> _closing_list;
};

#endif
