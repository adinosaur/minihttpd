//
// Date: 2015/10/08
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_HTTP_H
#define MINIHTTPD_HTTP_H

#include "HttpRequest.h"
#include "HttpResponse.h"

class HttpAuthorization;

//
// 实现Http协议但不拥有文件描述符
// 在析构时不能关闭文件描述符(sockfd)
//
class Http : public HttpBase
{
    public:
        Http(int connfd):
            _connfd(connfd),
            _request_flag(false),
            _response_flag(false),
            _http_request(),
            _http_response()
        {
        }
        
        ~Http()
        {
        }
        
        //
        // get a HttpRequest from socket.
        // storaged in HttpServer::_http_request.
        //
        void accept_request();
        
        //
        // handle HttpRequest.
        //
        void handle_request();
        
        //
        // send response
        //
        void send_response();
        
        //
        // 返回资源的绝对路径
        //
        string absolute_path();
        
        //
        // 判断请求的是否是目录文件
        // 必须在调用path_not_found函数之后调用
        //
        bool is_directory(const string& path);

        //
        // 判断请求的文件是否存在
        // 必须在调用accept_request函数之后调用
        //
        bool path_not_found(const string& path);

        //
        // 检测协议和函数是否被支持
        // 必须在调用accept_request函数之后调用
        //
        bool supported();

        //
        // 用户认证
        // 必须在调用accept_request函数之后调用
        //
        bool do_authorization();
        
        //
        // 返回目录下的index.html文件
        // 执行成功返回true，否则为false 
        //
        bool serve_index();
        
        //
        // 返回静态文件
        // 执行成功返回true，否则为false
        //
        bool serve_file(const string& path);
        
        //
        // 上传文件，PUT方法调用
        // 执行成功返回true，否则为false
        //
        bool put_file(const string& path);
        
        //
        // 执行CGI程序
        // 执行成功返回true，否则为false
        //
        bool execute_cgi(const string& path);
        
        //
        // 设置环境变量,执行CGI程序需要用到
        //
        void setenv();
        
        //
        // 400 Bad Request
        //
        void bad_request();
        
        //
        // 401 Authorization Required
        //
        void authorization_required();
        
        //
        // 404 Not found
        //
        void not_found();
        
        //
        // 500 HTTP-Internal Server Error
        //
        void internal_server_error();
        
        //
        // 501 Not implemented
        //
        void unimplemented();
        
        //
        // index
        //
        void index();
        
        //
        // 设置Http认证模块
        //
        static void set_auth(HttpAuthorization* auth);
        
    private:
        int _connfd;
        
        bool _request_flag;
        bool _response_flag;
        
        HttpRequest _http_request;
        HttpResponse _http_response;
        
        static HttpAuthorization* _http_authorization;
};

#endif

