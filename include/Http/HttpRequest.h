//
// Date: 2015/10/09
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_HTTPREQUEST_H
#define MINIHTTPD_HTTPREQUEST_H

//#include "../Logger/Logger.h"
#include "HttpBase.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <map>

using std::string;

//
// warpper for HTTP request.
//
class HttpRequest : public HttpBase
{
    public:
        HttpRequest():
            _method(HttpMethod::unknown),
            _version(HttpVersion::unknown),
            _path(),
            _query(),
            _headers(),
            _body()
        {
        }
        
        HttpRequest(HttpMethod m, HttpVersion v):
            _method(m),
            _version(v),
            _path(),
            _query(),
            _headers()
        {
        }
        
        HttpRequest(HttpMethod m, HttpVersion v, 
            const string& p, 
            const string& q):
            _method(m),
            _version(v),
            _path(p),
            _query(q),
            _headers()
        {
        }
        
        void set_method(const char* start, const char* end)
        {
            string m(start, end);
            auto it = str_to_method_map.find(m);
            if (it != str_to_method_map.end())
                _method = it->second;
            else
                _method = HttpMethod::unknown;
        }
        
        HttpMethod get_method()
        {
            return _method;
        }
        
        void set_version(const char* start, const char* end)
        {
            string v(start, end);
            auto it = str_to_version_map.find(v);
            if (it != str_to_version_map.end())
                _version = it->second;
            else
                _version = HttpVersion::unknown;
        }
        
        HttpVersion get_version()
        {
            return _version;
        }
        
        void set_path(const char* start, const char* end)
        {
            _path.assign(start, end);
        }
        
        string get_path()
        {
            return _path;
        }
        
        void set_query(const char* start, const char* end)
        {
            _query.assign(start, end);
        }
        
        string get_query()
        {
            return _query;
        }
        
        void add_header(const char* start, const char* end)
        {
            const char* p = std::find(start, end, ':');
            string key(start, p);
            
            p++;
            while (p < end && isspace(*p))
                p++;
            const char* e = end;
            while (e > p && isspace(*e))
                e--;
            
            string value(p, e+1);
            _headers[key] = value;
        }
        
        string get_header(const string& k)
        {
            string result;
            auto it = _headers.find(k);
            if (it != _headers.end())
                result = it->second;
            return result;
        }
        
        void append_to_body(const string& b)
        {
            _body.append(b);
        }

        void append_to_body(const char* b, const char* e)
        {
            _body.append(b, e);
        }
        
        const string& body()
        {
            return _body;
        }
        
        string str_method()
        {
            return method_to_str_map[_method];
        }
        
        string str_version()
        {
            return version_to_str_map[_version];
        }
        
        void print(std::ostream& os)
        {
            os << ">" << method_to_str_map[_method];
            os << " " << _path;
            if (!_query.empty())
                os << "?" << _query;
            os << " ";
            
            os << str_version();
            os << "\r\n";
            
            for (auto& header : _headers)
            {
                os << ">";
                os << header.first << ": " << header.second << "\r\n";
            }
            os << "\r\n";
            os << ">" << _body << std::endl;
        }
        
    private:
        HttpMethod _method;
        HttpVersion _version;
        string _path;
        string _query;
        std::map<string, string> _headers;
        string _body;
};

#endif

