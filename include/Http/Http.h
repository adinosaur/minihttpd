//
// Date: 2015/10/08
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_HTTP_H
#define MINIHTTPD_HTTP_H

#include "../Logger/Logger.h"
#include "../Noncopyable.h"
#include "../Helper.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

#include <unistd.h>
#include <assert.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using std::string;

#define DEFAULT_HTTP_ROOT_DIR "/var/www"

class Http
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
            close(_connfd);
        }
        
        //
        // serve a file
        //
        void serve_file()
        {
            char buf[1024];
            std::ifstream f(Http::_http_root_dir + _http_request.get_path());
            
            if (f)
            {
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
                _http_response.add_header("Content-Type", content_type);
                
                while (1)
                {
                    f.read(buf, sizeof(buf));
                    if (f.gcount() == 0)
                        break;
                    _http_response.append_to_body(buf, buf + f.gcount());
                }
                
                TRACE_LOG.logging("TRACE", "SERVE-FILE", _http_request.get_path(), "SUFFIX", suffix);
                _response_flag = true;
            }
            else
            {
                not_found();
                TRACE_LOG.logging("TRACE", "SERVE-FILE", "FILE-NOT-FOUND");
            }
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
            int n;
            
            // read http request line.
            n = Helper::get_line(_connfd, buf, sizeof(buf));
            
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
                n = Helper::get_line(_connfd, buf, sizeof(buf));
                
                beg = buf;
                end = beg + n;
                cur = beg;

                while (cur < end && (*cur) != '\n')
                    cur++;
                
                if (beg != cur)
                    _http_request.add_header(beg, cur);
                else
                    break;
            
                // skip over space.
                while (cur < end && isspace(*cur))
                    cur++;
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
            bool cgi = false;
            
            if (!supported())
            {
                unimplemented();
                return;
            }
            else 
            {
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
        
        //
        // send response
        //
        void send_response()
        {
            string buf = _http_response.to_string();
            write(_connfd, buf.data(), buf.size());
            _http_response.print(std::cout);
        }
    private:
        static string _http_root_dir;
        
        int _connfd;
        
        bool _request_flag;
        bool _response_flag;
        
        HttpRequest _http_request;
        HttpResponse _http_response;
};

string Http::_http_root_dir(DEFAULT_HTTP_ROOT_DIR);

#endif

