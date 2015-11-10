//
// Date: 2015/10/10
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_EVENTLOOP_H
#define MINIHTTPD_EVENTLOOP_H

#include "../Base/Logger.h"
#include "IOMultiplexing.h"
#include "Channel.h"
#include <vector>
#include <list>

//
// 静多态
//
template <typename IOMulti>
class EventLoop
{
    public:
        explicit EventLoop(int listenfd):
            _sockfd(listenfd),
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
        // 往EventLoop添加事件
        //
        void add_channel(Channel* channel)
        {
            _iomulti.add_channel(channel->fd(), channel);
            Logger::instance(Logger::TRACE)->logging(__FILE__, __LINE__, 
                "CHANNEL-ADD-IN-EVENTLOOP:" + std::to_string(channel->fd()));
        }

        //
        // 从EventLoop中删除事件
        //
        void del_channel(Channel* channel)
        {
            _iomulti.del_channel(channel->fd());
            Logger::instance(Logger::TRACE)->logging(__FILE__, __LINE__, 
                "CHANNEL-REMOVE-FROM-EVENTLOOP:" + std::to_string(channel->fd()));
        }

        //
        // 事件循环
        //
        void loop()
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
        
    private:
        int _sockfd;
        
        IOMulti _iomulti;
        std::vector<Channel*> _active_channel_list;
};

#endif

