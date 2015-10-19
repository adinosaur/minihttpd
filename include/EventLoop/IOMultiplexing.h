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
        // 虚析构
        //
        virtual ~IOMultiplexing() = default;
        
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
        
        //
        // 纯虚函数
        //
        virtual void loop(ChannelList& channel_list) = 0;
        
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
        // 虚函数
        // 重写基类的loop
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
            INFO_LOG.logging("INFO", "SELECT", "NREADY", nready);
            
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

#endif
