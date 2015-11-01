//
// Date: 2015/10/08
// Author: Dinosaur W.
//

#include "../include/TcpServer/TcpServer.h"
#include "../include/EventLoop/EventLoop.h"
#include "../include/EventLoop/IOMultiplexing.h"
#include "../include/Http/Http.h"
#include "../include/Http/HttpAuthorization.h"

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
