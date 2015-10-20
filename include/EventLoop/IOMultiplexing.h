//
// Date: 2015/10/19
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_IOMULTIPLEXING_H
#define MINIHTTPD_IOMULTIPLEXING_H

#include "../Logger/Logger.h"
#include "Channel.h"

#include <assert.h>
#include <vector>
#include <map>

//
// 实现IO复用，获取IO事件
//
class IOMultiplexing
{
    public:
        typedef std::vector<Channel*> ChannelList;
        
        //
        // 添加Channel对象
        //
        void add_channel(int fd, Channel* channel)
        {
            _channel_map[fd] = channel;
        }
        
        //
        // 删除Channel对象
        //
        void del_channel(int fd)
        {
            auto it = _channel_map.find(fd);
            if (it != _channel_map.end())
                _channel_map.erase(it);
        }
        
    protected:
        typedef std::map<int, Channel*> ChannelMap;
        
        // fd到Channel的映射
        ChannelMap _channel_map;
};


#include <sys/select.h>

//
// 使用Select作为底层的IO复用
//
class Select : public IOMultiplexing
{
    public:
        //
        // Select的loop
        //
        void loop(ChannelList& channel_list)
        {
            select(channel_list);
        }
        
        //
        // 简单封装的select
        // 将获取的IO事件放入vector中以参数形式返回
        //
        void select(ChannelList& channel_list)
        {
            fd_set rset;
            read_set(&rset);
            int maxfd = maxfd_in_channelMap();
            
            // 调用全局的select函数
            int nready = ::select(maxfd+1, &rset, NULL, NULL, NULL);
            
            // 日志
            INFO_LOG.logging("INFO", __FILE__, __LINE__, "SELECT", "NREADY", nready);
            
            for (auto& x : _channel_map)
            {
                if (FD_ISSET(x.first, &rset))
                {
                    channel_list.push_back(x.second);
                    
                    // 小优化
                    if (--nready <= 0)
                        break;
                }
            }
        }
    private:
        //
        // 找出_channel_map中最大的fd
        // 时间复杂度为O(nlgn)
        //
        int maxfd_in_channelMap()
        {
            assert(!_channel_map.empty());
            return (--_channel_map.end())->first;
        }
        
        //
        // 构造rset
        //
        void read_set(fd_set* set)
        {
            FD_ZERO(set);
            
            for (auto& x : _channel_map)
            {
                if ((x.second)->enable_reading())
                    FD_SET(x.first, set);
            }
        }
};

#include <poll.h>

//
// 使用Poll作为底层的IO复用
//

class Poll : public IOMultiplexing
{
    public:
        //
        // Poll的loop
        //
        void loop(ChannelList& channel_list)
        {
            poll(channel_list);
        }
        
        //
        // 简单封装的poll
        // 将获取的IO事件放入vector中以参数形式返回
        //
        void poll(ChannelList& channel_list)
        {
            std::vector<struct pollfd> fd_array;
            
            for (auto& x : _channel_map)
            {
                if (x.second->enable_reading())
                {
                    struct pollfd poll_fd;
                    poll_fd.fd = x.first;
                    poll_fd.events = POLLRDNORM;
                    fd_array.push_back(poll_fd);
                }
            }
            
            // 调用全局的poll函数
            int nready = ::poll(static_cast<struct pollfd *>(fd_array.data()), fd_array.size(), -1);
            
            // 日志
            INFO_LOG.logging("INFO", __FILE__, __LINE__, "POLL", "NREADY", nready);
            
            for (auto& x : fd_array)
            {
                if (x.revents & POLLRDNORM)
                {
                    channel_list.push_back(_channel_map[x.fd]);
                    
                    // 小优化
                    if (--nready <= 0)
                        break;
                }
            }
        }
};

#include <sys/epoll.h>

//
// 使用Epoll作为底层的IO复用
//

class Epoll : public IOMultiplexing
{
    public:
        //
        // Epoll的loop
        //
        void loop(ChannelList& channel_list)
        {
            epoll(channel_list);
        }
        
        //
        // 简单封装的epoll
        // 将获取的IO事件放入vector中以参数形式返回
        //
        void epoll(ChannelList& channel_list)
        {
            int max_events = 10;
            struct epoll_event ev;
            std::vector<struct epoll_event> events(max_events);
            
            int epollfd = epoll_create(10);
            if (epollfd == -1) 
            {
               ERROR_LOG.logging("ERROR", __FILE__, __LINE__, "EPOLL-CREATE");
               exit(1);
            }

            for (auto& x : _channel_map)
            {
                if (x.second->enable_reading())
                {
                    ev.events = EPOLLIN;
                    ev.data.fd = x.first;
                    
                    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, x.first, &ev) == -1) 
                    {
                        ERROR_LOG.logging("ERROR", __FILE__, __LINE__, "EPOLL-CTL");
                        exit(1);
                    }
                }
            }
            
            // 调用全局的epoll函数
            int nready = ::epoll_wait(epollfd, events.data(), max_events, -1);
            if (nready == -1) 
            {
                ERROR_LOG.logging("ERROR", __FILE__, __LINE__, "EPOLL-PWAIT");
                exit(1);
            }

            // 日志
            INFO_LOG.logging("INFO", __FILE__, __LINE__, "POLL", "NREADY", nready);
            
            for (int i = 0; i != nready; ++i)
                channel_list.push_back(_channel_map[events[i].data.fd]);
        }
};
#endif
