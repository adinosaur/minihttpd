#include "HttpResponse.h"
#include <algorithm>
#include <iostream>

using namespace std;

void HttpResponse::set_status_code(int code)
{
    _status_code = static_cast<HttpStatusCode>(code);
}

string HttpResponse::get_version()
{
    return version_to_str_map[_version];
}

void HttpResponse::append_to_body(const string& b)
{
    _body.append(b);
}

void HttpResponse::append_to_body(const char* b, const char* e)
{
    _body.append(b, e);
}

void HttpResponse::set_body(const string& body)
{
    _body = body;
}

const string& HttpResponse::get_body()
{
    return _body;
}

void HttpResponse::add_header(const string& k, const string& v)
{
    _headers[k] = v;
}

// 支持HTTP的HEAD方法
string HttpResponse::to_string_without_body()
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

string HttpResponse::to_string()
{
    string response = to_string_without_body();
    response += _body;
    return response;
}

void HttpResponse::print(std::ostream& os)
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
