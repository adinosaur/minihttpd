//
// Date: 2015/10/10
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_EVENTLOOP_H
#define MINIHTTPD_EVENTLOOP_H

#include "../Logger.h"
#include "IOMultiplexing.h"
#include "Channel.h"

#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#include <vector>
#include <memory>
#include <mutex>
#include <thread>

//
// 静多态
//
template <typename Protocol, typename IOMulti>
class EventLoop
{
    public:
        explicit EventLoop(int listenfd, std::mutex* m=nullptr):
            _sockfd(listenfd),
            _mutex(m),
            _iomulti(IOMulti()),
            _active_channel_list()
        {
        }
        
        ~EventLoop()
        {
        }
        
        // 禁止copy
        EventLoop(const EventLoop&) = delete;
        EventLoop& operator=(const EventLoop&) = delete;

        //
        // 封装::accept
        //
        int accept(int fd, struct sockaddr* addr, socklen_t* len)
        {
            if (_mutex != nullptr)
                _mutex->lock();
            
            int connfd = ::accept(fd, addr, len);
            
            if (_mutex != nullptr)
                _mutex->unlock();
            
            TRACE_LOG.logging(__FILE__, __LINE__, "ACCEPT-THREAD-ID", std::this_thread::get_id());
            return connfd;
        }

        void loop()
        {
            TRACE_LOG.logging(__FILE__, __LINE__, "THREAD-ID", std::this_thread::get_id());
            
            // 用监听文件fd创建Channel对象
            Channel listen_channel(_sockfd);
            
            // 设置新连接到时的处理函数
            listen_channel.set_enable_reading();
            listen_channel.set_read_callback([=]()
            {
                char client_addr[32];
                struct sockaddr_in cliaddr;
                socklen_t length;
                
                // accept
                int connfd = this->accept(listen_channel.fd(), (struct sockaddr*)(&cliaddr), &length);
                if (connfd < 0)
                {
                    ERROR_LOG.logging(__FILE__, __LINE__, "ACCEPT-ERROR");
                    exit(1);
                }
                
                // logger.
                inet_ntop(AF_INET, &cliaddr.sin_addr, client_addr, sizeof(client_addr));
                TRACE_LOG.logging(__FILE__, __LINE__, "NEW-CONNECTION", client_addr, ntohs(cliaddr.sin_port), connfd);
                
                // connect_channel必须是一个堆变量
                // 且生存期要到调用connect_channel中的callback中的del_channel(connfd)
                // 因此不使用智能指针，改用原始的指针和new创建对象
                // connect_channel的析构在其回调函数完成
                
                // 对于每一个新建立的连接
                // 用该连接的fd创建Channel对象
                Channel* connect_channel = new Channel(connfd);
                
                // 并设置read的Callback
                connect_channel->set_enable_reading();
                connect_channel->set_read_callback([=]()
                {
                    Protocol protocol(connfd);
                    
                    protocol.accept_request();
                    protocol.handle_request();
                    protocol.send_response();
                    
                    // 关闭TCP连接
                    _iomulti.del_channel(connfd);
                    delete connect_channel;
                    close(connfd);
                });
                
                // 最后将其添加至_iomulti.ChannelMap
                _iomulti.add_channel(connfd, connect_channel);
            });
            
            // 添加至_iomulti.ChannelMap.
            _iomulti.add_channel(_sockfd, &listen_channel);
            
            while (1)
            {
                // IO复用
                // 通过IO复用获取到IO事件
                _iomulti.loop(_active_channel_list);
                
                // swap技法清空vector
                std::vector<Channel*> channel_list;
                channel_list.swap(_active_channel_list);
                
                // 消息分发
                // 将获得的IO事件分发给各个文件描述符的事件处理函数
                for (auto channel : channel_list)
                    channel->handle_event();
            }
        }
        
    private:
        int _sockfd;
        std::mutex* _mutex;
        
        IOMulti _iomulti;
        std::vector<Channel*> _active_channel_list;
};

#endif

