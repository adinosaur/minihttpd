//
// Date: 2015/10/08
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_HTTPSERVER_H
#define MINIHTTPD_HTTPSERVER_H

#include "Noncopyable.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Helper.h"

#include <unistd.h>
#include <assert.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using std::string;

class HttpServer
{
    public:
        HttpServer(int connfd):
            _connfd(connfd),
            _request_flag(false),
            _response_flag(false),
            _http_request(),
            _http_response()
        {
        }
        
        ~HttpServer()
        {
            close(_connfd);
        }
        
        //
        // serve a file
        //
        void serve_file()
        {
            char buf[1024];
            std::ifstream f(_http_request.get_path());
            
            if (f)
            {
                _http_response.set_status_code(200);
                _http_response.add_header("Content-Type", "text/plain");
                
                while (1)
                {
                    f.read(buf, sizeof(buf));
                    if (f.gcount() == 0)
                        break;
                    _http_response.append_to_body(buf, buf + f.gcount());
                }
                
                _response_flag = true;
            }
            else
                not_found();
        }
        
        //
        // execute cgi program.
        //
        void execute_cgi()
        {
            std::cout << "execute_cgi() not implemented." << std::endl;
        }
        
        //
        // check for supported.
        // must called after accept_request().
        //
        bool supported()
        {
            assert(_request_flag);
            return _http_request.get_method() != HttpRequest::METHOD_UNKNOWN
                && _http_request.get_version() != HttpRequest::HTTPV_UNKONWN;
        }
        
        //
        // Not implemented
        //
        void unimplemented()
        {
            string body("");
             
            body += "<HTML><HEAD><TITLE>Method Not Implemented\r\n";
            body += "</TITLE></HEAD>\r\n";
            body += "<BODY><P>HTTP request method not supported.\r\n";
            body += "</BODY></HTML>\r\n";
            
            _http_response.set_status_code(501);
            _http_response.add_header("Content-Type", "text/html");
            _http_response.set_body(body);
            
            _response_flag = true;
        }
        
        //
        // Not found
        //
        void not_found()
        {
            string body("");
             
            body += "<HTML><TITLE>Not Found</TITLE>\r\n";
            body += "<BODY><P>404 NOT FOUND.\r\n";
            body += "</BODY></HTML>\r\n";
            
            _http_response.set_status_code(404);
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
             
            body += "<HTML><TITLE>Index</TITLE>\r\n";
            body += "<BODY><P>Hello World\r\n";
            body += "</BODY></HTML>\r\n";
            
            _http_response.set_status_code(200);
            _http_response.add_header("Content-Type", "text/html");
            _http_response.set_body(body);
            
            _response_flag = true;
        }
        
        //
        // get a HttpRequest from socket.
        // storaged in HttpServer::_http_request.
        //
        void accept_request()
        {
            char buf[1024];
            
            //pthread_detach(pthread_self());
            
            // read http request line.
            Helper::get_line(_connfd, buf, sizeof(buf));
            
            HttpRequest _http_request;
            
            char* beg = buf;
            char* end = beg + sizeof(buf);
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
            while (cur < end)
            {
                beg = cur;
                while (cur < end && (*cur) != '\n')
                    cur++;
                _http_request.add_header(beg, cur);
            
                // skip over space.
                while (cur < end && isspace(*cur))
                    cur++;
            }
            
            _request_flag = true;
        }
        
        //
        // handle HttpRequest.
        //
        void handle_request()
        {
            assert(_request_flag);
            bool cgi = false;
            
            if (!supported())
            {
                unimplemented();
                return;
            }
            else 
            {
                std::string file("/var/www");
                std::string query;
                
                if (_http_request.get_method() == HttpResponse::METHOD_POST ||
                    !_http_request.get_query().empty())
                    cgi = true;
                
                if (_http_request.get_path() == "/")
                {
                    index();
                }
                else
                {
                    if (!cgi)
                        serve_file();
                    else
                        execute_cgi();
                }
            }
        }
    private:
        int _connfd;
        
        bool _request_flag;
        bool _response_flag;
        
        HttpRequest _http_request;
        HttpResponse _http_response;
};

#endif

