//
// Date: 2015/10/09
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_HTTPREQUEST_H
#define MINIHTTPD_HTTPREQUEST_H

#include "HttpBase.h"
#include <string>
#include <map>

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
        
        void set_method(const char* start, const char* end);
        HttpMethod get_method();
        
        void set_version(const char* start, const char* end);
        HttpVersion get_version();
        
        void set_path(const char* start, const char* end);
        string get_path();
        
        void set_query(const char* start, const char* end);
        string get_query();
        
        void add_header(const char* start, const char* end);
        string get_header(const string& k);
        
        void append_to_body(const string& b);
        void append_to_body(const char* b, const char* e);
        
        const string& body();
        
        string str_method();
        string str_version();
        
        void print(std::ostream& os);
        
    private:
        HttpMethod _method;
        HttpVersion _version;
        std::string _path;
        std::string _query;
        std::map<string, string> _headers;
        std::string _body;
};

#endif

