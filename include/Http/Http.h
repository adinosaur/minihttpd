//
// Date: 2015/10/08
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_HTTP_H
#define MINIHTTPD_HTTP_H

#include "../Exception.h"
#include "../UrlCheck.h"
#include "../StrTime.h"
#include "../Logger.h"
#include "../Noncopyable.h"
#include "../Helper.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpAuthorization.h"

#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using std::string;

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
            //close(_connfd);
        }
        
        //
        // get a HttpRequest from socket.
        // storaged in HttpServer::_http_request.
        //
        void accept_request()
        {
            char buf[2048];
            int n;
            
            // read http request line.
            n = Helper::readline(_connfd, buf, sizeof(buf));
            
            TRACE_LOG.logging(__FILE__, __LINE__, "READLINE-BYTES", n);
            
            char* beg = buf;
            char* end = beg + n;
            char* cur = beg;
            
            // set HTTP request line.
            // get http method.
            while (cur < end && !isspace(*cur))
                cur++;
            _http_request.set_method(beg, cur);
            
            // skip over space.
            while (cur < end && isspace(*cur))
                cur++;
            
            // get http path.
            beg = cur;
            while (cur < end && !isspace(*cur) && *cur != '?')
                cur++;
            _http_request.set_path(beg, cur);
            
            // get http query string.
            if (*cur == '?')
                cur++;
            beg = cur;
            while (cur < end && !isspace(*cur))
                cur++;
            _http_request.set_query(beg, cur);
            
            // skip over space.
            while (cur < end && isspace(*cur))
                cur++;
            
            // get http version.
            beg = cur;
            while (cur < end && !isspace(*cur))
                cur++;
            _http_request.set_version(beg, cur);
            
            // skip over space.
            while (cur < end && isspace(*cur))
                cur++;
            
            // set HTTP request headers.
            while (true)
            {
                n = Helper::readline(_connfd, buf, sizeof(buf));
                //std::cout << buf << std::endl;
                
                beg = buf;
                end = beg + n;
                cur = beg;

                while (cur < end && (*cur) != '\n')
                    cur++;
                
                if (*beg != '\r' && *(beg+1) != '\n')
                    _http_request.add_header(beg, cur);
                else
                    break;
                
                // skip over space.
                while (cur < end && isspace(*cur))
                    cur++;
            }
            
            // read body
            string content_length = _http_request.get_header("Content-Length");
            if (content_length != "")
            {
                unsigned long length = std::stoul(content_length);
                while (length > 0)
                {
                    n = read(_connfd, buf, sizeof(buf));
                    _http_request.append_to_body(buf, buf + n);
                    length -= n;
                }
            }
            
            _request_flag = true;
            _http_request.print(std::cout);
        }
        
        //
        // handle HttpRequest.
        //
        void handle_request()
        {
            assert(_request_flag);
            bool success;
            string abs_path = absolute_path();
            
            // Not Supported
            if (!supported())
            {
                unimplemented();
                return;
            }
            
            // Not Found
            if (path_not_found(abs_path) && 
                _http_request.get_method() != HttpMethod::put)
            {
                not_found();
                return;
            }
            
            // 检查url，确保不会超出http服务器根目录
            if (!urlUnderRootDir(_http_request.get_path()))
            {
                not_found();
                return;
            }
            
            if (is_directory(abs_path))
            {
                if (!serve_index())
                    //server_directory();
                    index();
                return;
            }

            // 认证
            if (_http_authorization != nullptr)
            {
                if (!do_authorization())
                {
                    authorization_required();
                    return;
                }
            }

            switch (_http_request.get_method())
            {
                // GET
                case HttpMethod::get:
                    if (!_http_request.get_query().empty())
                        success = execute_cgi();
                    else
                        success = serve_file(abs_path);
                    break;
                
                // HEAD
                case HttpMethod::head:
                    if (!_http_request.get_query().empty())
                        success = execute_cgi();
                    else
                        success = serve_file(abs_path);
                    break;
                
                // PUT
                case HttpMethod::put:
                    success = put_file();
                    break;
                
                // POST
                case HttpMethod::post:
                    success = execute_cgi();
                    break;
                
                // unknown method
                case HttpMethod::unknown:
                    success = false;
                    break;
            }

            if (!success)
                internal_server_error();
        }
        
        //
        // send response
        //
        void send_response()
        {
            string buf;
            StrTime now;
            
            // 添加通用头部
            _http_response.add_header("Date", now.to_string("%a, %d %b %Y %H:%M:%S %Z"));
            
            // 设置body长度
            const string& body = _http_response.get_body();
            if (!body.empty())
                _http_response.add_header("Content-Length", std::to_string(_http_response.get_body().size()));
            
            if (_http_request.get_method() == HttpMethod::head)
                buf = _http_response.to_string_without_body();
            else
                buf = _http_response.to_string();
            
            write(_connfd, buf.data(), buf.size());
            _http_response.print(std::cout);
        }
        
        //
        // 返回资源的绝对路径
        //
        string absolute_path()
        {
            return HttpBase::HTTP_ROOT_DIR + _http_request.get_path();
        }
        
        //
        // 判断请求的是否是目录文件
        // 必须在调用path_not_found函数之后调用
        //
        bool is_directory(const string& path)
        {
            assert(_request_flag);
            
            struct stat st;
            stat(path.data(), &st);
            return S_ISDIR(st.st_mode);
        }
        
        //
        // 判断请求的文件是否存在
        // 必须在调用accept_request函数之后调用
        //
        bool path_not_found(const string& path)
        {
            assert(_request_flag);
            
            struct stat st;
            return stat(path.data(), &st) == -1;
        }
        
        //
        // 检测协议和函数是否被支持
        // 必须在调用accept_request函数之后调用
        //
        bool supported()
        {
            assert(_request_flag);
            return _http_request.get_method() != HttpMethod::unknown
                && _http_request.get_version() != HttpVersion::unknown;
        }
        
        //
        // 用户认证
        // 必须在调用accept_request函数之后调用
        //
        bool do_authorization()
        {
            assert(_http_authorization != nullptr);
            auto* user_passwd = _http_authorization->check(_http_request.get_path());
            
            // 不需要认证
            if (user_passwd == nullptr)
                return true;

            string auth_message = _http_request.get_header("Authorization");
            if (auth_message.empty())
            {
                HttpAuthorization::AuthMethod m = _http_authorization->auth_method();
                string auth_field;
                
                auth_field += _http_authorization->auth_name();
                auth_field += ("realm=\"" + user_passwd->path + "\"");
                
                switch (m)
                {
                    case HttpAuthorization::AuthMethod::basic:
                        break;
                    case HttpAuthorization::AuthMethod::digest:
                        // TODO auth-int
                        auth_field += ", qop=\"auth\"";
                        auth_field += ", nonce=\"uvmMi12NRtYpEj9nUQ6AaHx5scPbJF4e\"";
                        break;
                }
                
                _http_response.add_header("www-Authenticate", auth_field);
                return false;
            }
            
            // _http_authorization->auth是虚函数
            string method = _http_request.str_method();
            return _http_authorization->auth(method, auth_message, *user_passwd);
        }
        
        //
        // 返回目录下的index.html文件
        // 执行成功返回true，否则为false 
        //
        bool serve_index()
        {
            // 默认搜索顺序
            std::vector<string> index_list = {"index.html", "index.htm", "home.html"};
            string abs_path = absolute_path();
            string file;
            
            for (auto filename : index_list)
            {
                if (abs_path.back() == '/')
                    file = abs_path + filename;
                else
                    file = abs_path + "/" + filename;
                
                if (!path_not_found(file))
                {
                    serve_file(file);
                    return true;
                }
            }
            return false;
        }
        
        //
        // 返回静态文件
        // 执行成功返回true，否则为false
        //
        bool serve_file(const string& path)
        {
            try
            {
                char buf[2048];
                std::ifstream f(path);
                
                if (!f)
                    throw ServeFileException(__FILE__, __LINE__, 
                        "SERVE-FILE-OPEN-ERROR", _http_request.get_path());

                string suffix;
                
                // get suffix name
                string path = _http_request.get_path();
                int i = path.size() - 1;
                while (path[i] != '.' && path[i] != '/')
                    i--;
                
                string content_type;
                if (path[i] == '/')
                    content_type = "text/plain";
                if (path[i] == '.')
                {
                    suffix = path.substr(i);
                    
                    auto it = HttpResponse::content_type_map.find(suffix);
                    if (it != HttpResponse::content_type_map.end())
                        content_type = it->second;
                    else
                        content_type = "text/plain";
                }
                
                _http_response.set_status_code(200);
                _http_response.add_header("Content-type", content_type);
                
                while (1)
                {
                    f.read(buf, sizeof(buf));
                    if (f.gcount() == 0)
                        break;
                    _http_response.append_to_body(buf, buf + f.gcount());
                }
                
                TRACE_LOG.logging(__FILE__, __LINE__, "SUFFIX", suffix);
            
                _response_flag = true;
                return true;
            } 
            catch (ServeFileException e)
            {
                ERROR_LOG.logging(e.filename, e.line_num, e.error_type, e.error_info);
                return false;
            }
        }
        
        //
        // 上传文件，PUT方法调用
        // 执行成功返回true，否则为false
        //
        bool put_file()
        {
            try
            {
                std::ofstream f;
                f.open(HttpBase::HTTP_ROOT_DIR + _http_request.get_path());
                if (!f)
                    throw PutFileException(__FILE__, __LINE__, 
                        "PUT-FILE-OPEN-ERROR", _http_request.get_path());
                
                // put file
                f << _http_request.body();
                
                string url("http://");
                url += _http_request.get_header("Host");
                url += _http_request.get_path();
                _http_response.set_body(url);
                
                return true;
            }
            catch (PutFileException e)
            {
                ERROR_LOG.logging(e.filename, e.line_num, e.error_type, e.error_info);
                return false;
            }
        }
        
        //
        // 执行CGI程序
        // 执行成功返回true，否则为false
        //
        bool execute_cgi()
        {
            pid_t pid;
            int cgi_input[2];
            int cgi_output[2];

            try
            {
                if (pipe(cgi_input) == -1)
                    throw ExecuteCGIException(__FILE__, __LINE__, "CREATE-PIPE", "cgi_input");
                
                if (pipe(cgi_output) == -1)
                    throw ExecuteCGIException(__FILE__, __LINE__, "CREATE-PIPE", "cgi_output");

                pid = fork();
                if (pid != 0)
                {
                    // parent
                    char buf[2048];
                    int status;
                    int nbyte;
                    const string &request_body = _http_request.body();
                    
                    close(cgi_output[1]);
                    close(cgi_input[0]);
                    
                    // 如果request有body数据则通过cgi-input发送给CGI进程
                    if (!request_body.empty())
                    {
                        nbyte = write(cgi_input[1], request_body.data(), request_body.size());
                        if (nbyte == -1)
                            throw ExecuteCGIException(__FILE__, __LINE__, "WRITE-PIPE", "cgi_input");
                    }
                    
                    // XXX 必须此时关闭
                    close(cgi_input[1]);
                    
                    _http_response.set_status_code(200);
                    
                    // 读取响应头
                    while ((nbyte = Helper::readline(cgi_output[0], buf, sizeof(buf))) > 0)
                    {
                        const char *beg = buf;
                        const char *cur = beg;
                        const char *end = buf + nbyte;
                        const char *rcur = nullptr;
                        
                        if (*beg == '\r' && *(beg+1) == '\n')
                            break;
                        
                        // skip over space.
                        while (cur < end && isspace(*cur))
                            cur++;
                        beg = cur;
                        
                        while (cur < end && *cur != ':')
                            cur++;
                        
                        rcur = cur;
                        // skip over space.
                        while (rcur > beg && isspace(*rcur))
                            rcur--;
                        
                        string key(beg, rcur);
                        
                        cur++;
                        
                        // skip over space.
                        while (cur < end && isspace(*cur))
                            cur++;
                        beg = cur;
                        
                        rcur = end - 1;
                        // skip over space.
                        while (rcur > beg && isspace(*rcur))
                            rcur--;
                        
                        string val(beg, rcur+1);
                        
                        TRACE_LOG.logging(__FILE__, __LINE__, "KEY", key, "VAL", val);
                        _http_response.add_header(key, val);
                    }
                    
                    // 读取body
                    while (1)
                    {
                        nbyte = read(cgi_output[0], buf, sizeof(buf));
                        if (nbyte == -1)
                            throw ExecuteCGIException(__FILE__, __LINE__, "READ-PIPE", "cgi_output");
                        
                        if (nbyte == 0)
                            break;
                        
                        _http_response.append_to_body(buf, buf + nbyte);
                    }
                    
                    _response_flag = true;
                    
                    close(cgi_output[0]);
                    
                    waitpid(pid, &status, 0);
                    if (status != 0)
                        throw ExecuteCGIException(__FILE__, __LINE__, "EXECUTE-CGI", "cgi error");
                }
                else
                {
                    // child
                    dup2(cgi_output[1], 1);
                    dup2(cgi_input[0], 0);
                    close(cgi_output[0]);
                    close(cgi_input[1]);
                    
                    // 设置环境变量
                    setenv();
                    
                    string path = HttpBase::HTTP_ROOT_DIR + _http_request.get_path();
                    TRACE_LOG.logging(__FILE__, __LINE__, "PATH", path);
                    
                    if (execl(path.data(), path.data(), NULL) == -1)
                    {
                        ERROR_LOG.logging(__FILE__, __LINE__, "EXECL", path);
                        exit(1);
                    }
                    //exit(0);
                }
                
                return true;
            }
            catch (ExecuteCGIException e)
            {
                ERROR_LOG.logging(e.filename, e.line_num, e.error_type, e.error_info);
                return false;
            }
        }
        
        //
        // 设置环境变量,执行CGI程序需要用到
        //
        void setenv()
        {
            string server_protocol(_http_request.str_version());
            string request_method(_http_request.str_method());
            string http_accept(_http_request.get_header("Accept"));
            string http_user_agent(_http_request.get_header("User-Agent"));
            string http_referer(_http_request.get_header("Referer"));
            string script_name(_http_request.get_path());
            string query_string(_http_request.get_query());
            string content_length(_http_request.get_header("Content-Length"));
            
            string host(_http_request.get_header("Host"));
            string server_name;
            string server_port;
            
            auto it = host.begin();
            for (; it != host.end(); ++it)
                if (*it == ':')
                    break;
            server_name = string(host.begin(), it);
            
            // 默认为80端口
            if (it == host.end())
                server_port = "80";
            else
                server_port = std::move(string(it+1, host.end()));
            
            if (!server_name.empty())
                ::setenv("SERVER_NAME", server_name.data(), 0);
            
            if (!server_port.empty())
                ::setenv("SERVER_PORT", server_port.data(), 0);
            
            if (!server_protocol.empty())
                ::setenv("SERVER_PROTOCOL", server_protocol.data(), 0);
            
            if (!request_method.empty())
                ::setenv("REQUEST_METHOD", request_method.data(), 0);
            
            if (!http_accept.empty())
                ::setenv("HTTP_ACCEPT", http_accept.data(), 0);
            
            if (!http_user_agent.empty())
                ::setenv("HTTP_USER_AGENT", http_user_agent.data(), 0);
            
            if (!http_referer.empty())
                ::setenv("HTTP_REFERER", http_referer.data(), 0);
            
            if (!script_name.empty())
                ::setenv("SCRIPT_NAME", script_name.data(), 0);
            
            if (!query_string.empty())
                ::setenv("QUERY_STRING", query_string.data(), 0);
            
            if (!content_length.empty())
                ::setenv("CONTENT_LENGTH", content_length.data(), 0);
        }
        
        //
        // 400 Bad Request
        //
        void bad_request()
        {
            string body("");
             
            body += "<HTML><HEAD><TITLE>Bad Request</TITLE></HEAD>\r\n";
            body += "<BODY><P>400 Bad Request.</P>\r\n";
            body += "</BODY></HTML>\r\n";
            
            _http_response.set_status_code(400);
            _http_response.add_header("Content-Type", "text/html");
            _http_response.set_body(body);
            
            _response_flag = true;
        }
        
        //
        // 401 Authorization Required
        //
        void authorization_required()
        {
            string body("");
             
            body += "<HTML><HEAD><TITLE>Authorization Required</TITLE></HEAD>\r\n";
            body += "<BODY><P>401 Authorization Required.</P>\r\n";
            body += "</BODY></HTML>\r\n";
            
            _http_response.set_status_code(401);
            _http_response.add_header("Content-Type", "text/html");
            _http_response.set_body(body);
            
            _response_flag = true;
            ERROR_LOG.logging(__FILE__, __LINE__, "AUTHORIZATION-REQUIRED", _http_request.get_path());
        }
        
        //
        // 404 Not found
        //
        void not_found()
        {
            string body("");
             
            body += "<HTML><HEAD><TITLE>Not Found</TITLE></HEAD>\r\n";
            body += "<BODY><P>404 NOT FOUND.</P>\r\n";
            body += "</BODY></HTML>\r\n";
            
            _http_response.set_status_code(404);
            _http_response.add_header("Content-Type", "text/html");
            _http_response.set_body(body);
            
            _response_flag = true;
            ERROR_LOG.logging(__FILE__, __LINE__, "NOT-FOUND", _http_request.get_path());
        }
        
        //
        // 500 HTTP-Internal Server Error
        //
        void internal_server_error()
        {
            string body("");
             
            body += "<HTML><HEAD><TITLE>HTTP-Internal Server Error</TITLE></HEAD>\r\n";
            body += "<BODY><P>500 HTTP-Internal Server Error.</P>\r\n";
            body += "</BODY></HTML>\r\n";
            
            _http_response.set_status_code(500);
            _http_response.add_header("Content-Type", "text/html");
            _http_response.set_body(body);
            
            _response_flag = true;
        }
        
        //
        // 501 Not implemented
        //
        void unimplemented()
        {
            string body("");
             
            body += "<HTML><HEAD><TITLE>Method Not Implemented</TITLE></HEAD>\r\n";
            body += "<BODY><P>HTTP request method not supported.</P>\r\n";
            body += "</BODY></HTML>\r\n";
            
            _http_response.set_status_code(501);
            _http_response.add_header("Content-Type", "text/html");
            _http_response.set_body(body);
            
            _response_flag = true;
        }
        
        //
        // index
        //
        void index()
        {
            string body("");
             
            body += "<HTML><HEAD><TITLE>Index</TITLE></HEAD>\r\n";
            body += "<BODY><P>Hello World.</P>\r\n";
            body += "</BODY></HTML>\r\n";
            
            _http_response.set_status_code(200);
            _http_response.add_header("Content-Type", "text/html");
            _http_response.set_body(body);
            
            _response_flag = true;
        }
        
        //
        // 设置Http认证模块
        //
        static void set_auth(HttpAuthorization* auth)
        {
            _http_authorization = auth;
        }
        
    private:
        int _connfd;
        
        bool _request_flag;
        bool _response_flag;
        
        HttpRequest _http_request;
        HttpResponse _http_response;
        
        static HttpAuthorization* _http_authorization;
};

HttpAuthorization* Http::_http_authorization = nullptr;

#endif

