//
// Date: 2015/10/09
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_HTTPRESPONSE_H
#define MINIHTTPD_HTTPRESPONSE_H

#include "HttpBase.h"

#include <string>
#include <map>
#include <algorithm>
#include <iostream>

using std::string;

//
// warpper for HTTP response.
// TODO only support HTTP/1.0 now.
//
class HttpResponse : public HttpBase
{
    public:
        HttpResponse():
            _version(HTTPV_10),
            _status_code(HTTP_STATUS_CODE_UNKNOWN),
            _headers(),
            _body()
        {
        }
        
        HttpResponse(int status_code, const string& body):
            _version(HTTPV_10),
            _status_code(status_code),
            _headers(),
            _body(body)
        {
        }
        
        void set_status_code(int code)
        {
            _status_code = code;
        }
        
        string get_version()
        {
            switch (_version)
            {
                case HTTPV_09:
                    return "HTTP/0.9";
                case HTTPV_10:
                    return "HTTP/1.0";
                case HTTPV_11:
                    return "HTTP/1.1";
                case HTTPV_20:
                    return "HTTP/2.0";
                default:
                    return "HTTP/1.0";
            }
        }

        void append_to_body(const string& b)
        {
            _body.append(b);
        }

        void append_to_body(char* b, char* e)
        {
            _body.append(b, e);
        }

        void set_body(const string& body)
        {
            _body = body;
        }
        
        void add_header(const string& k, const string& v)
        {
            _headers[k] = v;
        }
        
        string to_string()
        {
            string response;
            response += "HTTP/1.0 ";
            switch(_status_code)
            {
                case HTTP_STATUS_CODE_200:
                    response += "200 OK\r\n";
                    break;
                case HTTP_STATUS_CODE_301:
                    response += "301 Moved Permanently\r\n";
                    break;
                case HTTP_STATUS_CODE_400:
                    response += "400 Bad Request\r\n";
                    break;
                case HTTP_STATUS_CODE_404:
                    response += "404 Not Found\r\n";
                    break;
                case HTTP_STATUS_CODE_500:
                    response += "500 Internal Server Error\r\n";
                    break;
                case HTTP_STATUS_CODE_501:
                    response += "501 Not Implemented\r\n";
                    break;
            }
            
            for (auto& it : _headers)
            {
                response += it.first;
                response += ": ";
                response += it.second;
                response += "\r\n";
            }
            response += "\r\n";
            response += _body;
            
            return response;
        }
        
        void print(std::ostream& os)
        {
            string response = to_string();
            
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

            os << "<" << string(cur+2, end);
            os << std::endl;
        }
        
    private:
        int _version;
        int _status_code;
        std::map<string, string> _headers;
        string _body;
};

#endif
