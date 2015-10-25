minihttpd
========

##索引

* [基本介绍][base]
* [基本使用][base usage]
* [详细使用][advance usage]

[base]:
##基本介绍
基于c++11和Linux socket API实现的http服务器

* 实现了基本的http方法：GET，HEAD，POST，PUT
* 后端执行CGI程序
* http认证：基本认证、摘要认证（未实现）
* 可指定底层的IO复用模块（Select、Poll、Epoll）
* 作为工具实现的有Base64编码、Logger日志、Trie树（用来快速判断一个url是否需要认证）等等。

[base usage]:
## 基本使用
```c++
#include "../include/EventLoop/EventLoop.h"
#include "../include/EventLoop/IOMultiplexing.h"
#include "../include/Http/Http.h"

int main()
{    
    // 创建Http服务器
    EventLoop<Http> httpServer(8080);
        
    // IO循环
    httpServer.loop();
    return 0;
}
```

[advance usage]:
## 详细使用
```
#include "../include/EventLoop/EventLoop.h"
#include "../include/EventLoop/IOMultiplexing.h"
#include "../include/Http/Http.h"
#include "../include/Http/HttpAuthorization.h"

int main()
{
    // 设置http服务器的根目录
    Http::set_root_dir("/home/dinosaur/www");
    
    // 设置权限目录
    BasicAuth auth;
    auth.add("/root", "root", "12345");
    Http::set_auth(&auth);
    
    // 创建Http服务器
    EventLoop<Http> httpServer(8080);
    
    // 指定底层IO复用
    //EventLoop<Http, Epoll> httpServer(8080);
    
    // IO循环
    httpServer.loop();
    return 0;
}
```