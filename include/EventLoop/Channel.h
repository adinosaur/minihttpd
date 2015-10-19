//
// Date: 2015/10/19
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_CHANNEL_H
#define MINIHTTPD_CHANNEL_H

#include "EventLoop.h"
#include <functional>

//
// Channel类将fd和Callback函数绑定在一起
//
class Channel
{
    public:
        typedef std::function<void()> EventCallback;
        
        explicit Channel(int fd):
            _fd(fd),
            _enable_reading(false),
            _enable_writing(false)
        {
        }
        
        void handle_event()
        {
            if (_enable_reading && _read_callback)
                _read_callback();

            if (_enable_writing && _write_callback)
                _write_callback();
        }
        
        int fd() const
        {
            return _fd;
        }
        
        bool enable_reading()
        {
            return _enable_reading;
        }
        
        bool enable_writing()
        {
            return _enable_writing;
        }
        
        void set_enable_reading()
        {
            _enable_reading = true;
        }
        
        void set_enable_writing()
        {
            _enable_writing = true;
        }
        
        void set_read_callback(const EventCallback& cb)
        {
            _read_callback = cb;
        }
        
        void set_write_callback(const EventCallback& cb)
        {
            _write_callback = cb;
        }
        
        void set_error_callback(const EventCallback& cb)
        {
            _error_callback = cb;
        }
    private:
        int _fd;
        
        bool _enable_reading;
        bool _enable_writing;
        
        EventCallback _read_callback;
        EventCallback _write_callback;
        EventCallback _error_callback;
};

#endif
