minihttpd
========

基于c++11和socket API实现的web服务器

## 使用方法
```c++
#include "../include/EventLoop/EventLoop.h"
#include "../include/EventLoop/IOMultiplexing.h"
#include "../include/Http/Http.h"

int main()
{
    // 设置http服务器的根目录
    Http::set_root_dir("/home/dinosaur");
    
    // 创建Http服务器
    EventLoop<Http> httpServer(8080);
    
    // 指定底层IO复用
    //EventLoop<Http, Epoll> httpServer(8080);
    
    // IO循环
    httpServer.loop();
    return 0;
}
```