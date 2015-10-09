//
// Date: 2015/10/09
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_HTTPREQUEST_H
#define MINIHTTPD_HTTPREQUEST_H

#include "HttpBase.h"

#include <string>
#include <map>
#include <algorithm>

using std::string;

//
// warpper for HTTP request.
//
class HttpRequest : public HttpBase
{
    public:
        HttpRequest():
            _method(METHOD_UNKNOWN),
            _version(HTTPV_UNKONWN),
            _path(),
            _query(),
            _headers()
        {
        }
        
        HttpRequest(int m, int v):
            _method(m),
            _version(v),
            _path(),
            _query(),
            _headers()
        {
        }
        
        HttpRequest(int m, int v, 
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
            // TODO now only GET, POST supported.
            string m(start, end);
            if (m == "GET")
                _method = METHOD_GET;
            else if (m == "POST")
                _method = METHOD_POST;
            else
                _method = METHOD_UNKNOWN;
        }
        
        int get_method()
        {
            return _method;
        }
        
        void set_version(const char* start, const char* end)
        {
            string v(start, end);
            if (v == "HTTP/0.9")
                _version = HTTPV_09;
            else if (v == "HTTP/1.0")
                _version = HTTPV_10;
            else if (v == "HTTP/1.1")
                _version = HTTPV_11;
            else if (v == "HTTP/2.0")
                _version = HTTPV_20;
            else
                _version = HTTPV_UNKONWN;
        }
        
        int get_version()
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
        
    private:
        int _method;
        int _version;
        string _path;
        string _query;
        std::map<string, string> _headers;
};

#endif

