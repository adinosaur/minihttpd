minihttpd
========

## About
基于c++11和Linux socket API实现的http服务器

1. 实现了基本的http方法：GET，HEAD，POST，PUT
2. 后端执行CGI程序
3. http认证：基本认证、摘要认证
4. 可指定底层的IO复用模块（Select、Poll、Epoll）
5. 作为工具实现的有Base64编码、Logger日志、Trie树（用来快速判断一个url是否需要认证）等等。

## Basic Usage
```c++
#include "../include/Http/Http.h"
#include "../include/TcpServer/TcpServer.h"

int main()
{    
    // 创建Http服务器, 4个工作线程
    TcpServer<Http> httpServer(8080, 4);
        
    // IO循环
    httpServer.run();
    return 0;
}
```

## Advanced Usage
```c++
#include "../include/Http/Http.h"
#include "../include/TcpServer/TcpServer.h"
#include "../include/Http/HttpAuthorization.h"
#include "../include/EventLoop/IOMultiplexing.h"

int main()
{
    // 设置http服务器的根目录
    Http::set_root_dir("/home/dinosaur/www");
    
    // 设置权限目录
    // Basic认证
    //BasicAuth auth;
    //auth.add("/root", "root", "12345");
    //Http::set_auth(&auth);
    
    DigestAuth auth;
    auth.add("/root", "root", "12345");
    Http::set_auth(&auth);
    
    // 创建Http服务器
    TcpServer<Http> httpServer(8080, 4);
    
    // 指定底层IO复用
    //TcpServer<Http, Epoll> httpServer(8080, 4);
    
    // IO循环
    httpServer.run();
    return 0;
}
```
