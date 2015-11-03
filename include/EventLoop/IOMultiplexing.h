//
// Date: 2015/10/19
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_IOMULTIPLEXING_H
#define MINIHTTPD_IOMULTIPLEXING_H

#include <vector>
#include <map>

class Channel;

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
        void add_channel(int fd, Channel* channel);
        
        //
        // 删除Channel对象
        //
        void del_channel(int fd);
        
    protected:
        typedef std::map<int, Channel*> ChannelMap;
        
        // fd到Channel的映射
        ChannelMap _channel_map;
};

//
// 使用Select作为底层的IO复用
//
class Select : public IOMultiplexing
{
    public:
        //
        // Select的loop
        //
        void loop(ChannelList& channel_list);
        
        //
        // 简单封装的select
        // 将获取的IO事件放入vector中以参数形式返回
        //
        void select(ChannelList& channel_list);
    private:
        //
        // 找出_channel_map中最大的fd
        // 时间复杂度为O(nlgn)
        //
        int maxfd_in_channelMap();
        
        //
        // 构造rset
        //
        void read_set(fd_set* set);
};

//
// 使用Poll作为底层的IO复用
//

class Poll : public IOMultiplexing
{
    public:
        //
        // Poll的loop
        //
        void loop(ChannelList& channel_list);
        
        //
        // 简单封装的poll
        // 将获取的IO事件放入vector中以参数形式返回
        //
        void poll(ChannelList& channel_list);
};

//
// 使用Epoll作为底层的IO复用
//

class Epoll : public IOMultiplexing
{
    public:
        //
        // Epoll的loop
        //
        void loop(ChannelList& channel_list);
        
        //
        // 简单封装的epoll
        // 将获取的IO事件放入vector中以参数形式返回
        //
        void epoll(ChannelList& channel_list);
};
#endif
