//
// Date: 2015/10/09
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_HTTPRESPONSE_H
#define MINIHTTPD_HTTPRESPONSE_H

#include "HttpBase.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <map>

using std::string;

//
// warpper for HTTP response.
// TODO only support HTTP/1.0 now.
//
class HttpResponse : public HttpBase
{
    public:
        HttpResponse():
            _version(HttpVersion::v11),
            _status_code(HttpStatusCode::unknown),
            _headers(),
            _body()
        {
        }
        
        HttpResponse(int status_code, const string& body):
            _version(HttpVersion::v11),
            _status_code(static_cast<HttpStatusCode>(status_code)),
            _headers(),
            _body(body)
        {
        }
        
        void set_status_code(int code)
        {
            _status_code = static_cast<HttpStatusCode>(code);
        }
        
        string get_version()
        {
            return version_to_str_map[_version];
        }

        void append_to_body(const string& b)
        {
            _body.append(b);
        }

        void append_to_body(const char* b, const char* e)
        {
            _body.append(b, e);
        }

        void set_body(const string& body)
        {
            _body = body;
        }
        
        const string& get_body()
        {
            return _body;
        }
        
        void add_header(const string& k, const string& v)
        {
            _headers[k] = v;
        }
        
        // 支持HTTP的HEAD方法
        string to_string_without_body()
        {
            string response;
            response += version_to_str_map[_version];
            response += " ";
            response += status_to_str_map[_status_code];
            response += "\r\n";
            
            for (auto& it : _headers)
            {
                response += it.first;
                response += ": ";
                response += it.second;
                response += "\r\n";
            }
            response += "\r\n";
            return response;
        }
        
        string to_string()
        {
            string response = to_string_without_body();
            response += _body;
            return response;
        }
        
        void print(std::ostream& os)
        {
            //string response = to_string();
            string response = to_string_without_body();
            
            auto beg = response.begin();
            auto cur = beg;
            auto end = response.end();
            
            for (; cur != end; ++cur)
            {
                if (*cur == '\r' && *(cur+1) == '\n')
                {
                    os << "<";
                    os << string(beg, cur+2);
                    
                    // "\r\n\r\n" meaning headers end.
                    if (beg == cur)
                        break;
                    else
                        beg = cur+2;
                }
            }

            //os << "<" << string(cur+2, end);
            os << std::endl;
        }
        
    private:
        HttpVersion _version;
        HttpStatusCode _status_code;
        std::map<string, string> _headers;
        string _body;
};

#endif
