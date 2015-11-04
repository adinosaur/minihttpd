
#include "Channel.h"

Channel::Channel(int fd):
    _fd(fd),
    _enable_reading(false),
    _enable_writing(false)
{
}

void Channel::handle_event()
{
    if (_enable_reading && _read_callback)
        _read_callback();

    if (_enable_writing && _write_callback)
        _write_callback();
}

int Channel::fd() const
{
    return _fd;
}

bool Channel::enable_reading()
{
    return _enable_reading;
}

bool Channel::enable_writing()
{
    return _enable_writing;
}

void Channel::set_enable_reading()
{
    _enable_reading = true;
}

void Channel::set_enable_writing()
{
    _enable_writing = true;
}

void Channel::set_read_callback(const EventCallback& cb)
{
    _read_callback = cb;
}

void Channel::set_write_callback(const EventCallback& cb)
{
    _write_callback = cb;
}

void Channel::set_error_callback(const EventCallback& cb)
{
    _error_callback = cb;
}
