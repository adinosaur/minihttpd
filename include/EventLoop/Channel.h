//
// Date: 2015/10/19
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_CHANNEL_H
#define MINIHTTPD_CHANNEL_H

#include <functional>

//
// Channel类将fd和Callback函数绑定在一起
// Channel负责将事件分发给注册在该文件描述符上的CallBack函数
// Channel并不拥有一个文件描述符，不会在析构时关闭这个文件描述符
//
class Channel
{
    public:
        typedef std::function<void()> EventCallback;
        
        Channel(int fd);
        
        void handle_event();
        int fd() const;
        
        bool enable_reading();
        bool enable_writing();
        
        void set_enable_reading();
        void set_enable_writing();
        
        void set_read_callback(const EventCallback& cb);
        void set_write_callback(const EventCallback& cb);
        void set_error_callback(const EventCallback& cb);
    private:
        int _fd;
        
        bool _enable_reading;
        bool _enable_writing;
        
        EventCallback _read_callback;
        EventCallback _write_callback;
        EventCallback _error_callback;
};

#endif
