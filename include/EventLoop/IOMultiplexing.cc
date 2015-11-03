//
// Date: 2015/10/19
// Author: Dinosaur W.
//
#include "IOMultiplexing.h"
#include "../Base/Logger.h"
#include "Channel.h"

#include <sys/select.h>
#include <poll.h>
#include <sys/epoll.h>

#include <assert.h>
#include <vector>
#include <map>

//
// 添加Channel对象
//
void IOMultiplexing::add_channel(int fd, Channel* channel)
{
    _channel_map[fd] = channel;
}

//
// 删除Channel对象
//
void IOMultiplexing::del_channel(int fd)
{
    auto it = _channel_map.find(fd);
    if (it != _channel_map.end())
        _channel_map.erase(it);
}

//
// Select的loop
//
void Select::loop(ChannelList& channel_list)
{
    select(channel_list);
}

//
// 简单封装的select
// 将获取的IO事件放入vector中以参数形式返回
//
void Select::select(ChannelList& channel_list)
{
    fd_set rset;
    read_set(&rset);
    int maxfd = maxfd_in_channelMap();
    
    // 调用全局的select函数
    int nready = ::select(maxfd+1, &rset, NULL, NULL, NULL);
    
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

//
// 找出_channel_map中最大的fd
// 时间复杂度为O(nlgn)
//
int Select::maxfd_in_channelMap()
{
    assert(!_channel_map.empty());
    return (--_channel_map.end())->first;
}

//
// 构造rset
//
void Select::read_set(fd_set* set)
{
    FD_ZERO(set);
    
    for (auto& x : _channel_map)
    {
        if ((x.second)->enable_reading())
            FD_SET(x.first, set);
    }
}

//
// Poll的loop
//
void Poll::loop(ChannelList& channel_list)
{
    poll(channel_list);
}

//
// 简单封装的poll
// 将获取的IO事件放入vector中以参数形式返回
//
void Poll::poll(ChannelList& channel_list)
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

//
// Epoll的loop
//
void Epoll::loop(ChannelList& channel_list)
{
    epoll(channel_list);
}

//
// 简单封装的epoll
// 将获取的IO事件放入vector中以参数形式返回
//
void Epoll::epoll(ChannelList& channel_list)
{
    int max_events = 10;
    struct epoll_event ev;
    std::vector<struct epoll_event> events(max_events);
    
    int epollfd = epoll_create(10);
    if (epollfd == -1) 
    {
       Logger::instance(Logger::ERROR)->logging(__FILE__, __LINE__, "EPOLL-CREATE");
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
                Logger::instance(Logger::ERROR)->logging(__FILE__, __LINE__, "EPOLL-CTL");
                exit(1);
            }
        }
    }
    
    // 调用全局的epoll函数
    int nready = ::epoll_wait(epollfd, events.data(), max_events, -1);
    if (nready == -1) 
    {
        Logger::instance(Logger::ERROR)->logging(__FILE__, __LINE__, "EPOLL-PWAIT");
        exit(1);
    }
    
    for (int i = 0; i != nready; ++i)
        channel_list.push_back(_channel_map[events[i].data.fd]);
}
