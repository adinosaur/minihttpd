//
// Date: 2015/10/09
// Author: Dinosaur W.
//

#include "../src/include/HttpRequest.h"

#include <assert.h>
#include <ctype.h>

#include <iostream>
#include <string>

using namespace std;

void test_get()
{
    string buf =   "GET /color.cc HTTP/1.1\r\n"
                    "Connection: keep-alive\r\n"
                    "Pragma: no-cache\r\n"
                    "Cache-Control: no-cache\r\n"
                    "Host: 127.0.0.1:8080\r\n\r\n";
    
    HttpRequest http_request;
    
    char* beg = const_cast<char*>(buf.data());
    char* end = beg + buf.size();
    char* cur = beg;
    
    // set HTTP request line.
    while (cur < end && !isspace(*cur))
        cur++;
    http_request.set_method(beg, cur);
    
    while (cur < end && isspace(*cur))
        cur++;
    
    beg = cur;
    while (cur < end && !isspace(*cur))
        cur++;
    http_request.set_path(beg, cur);
    
    while (cur < end && isspace(*cur))
        cur++;
    
    beg = cur;
    while (cur < end && !isspace(*cur))
        cur++;
    http_request.set_version(beg, cur);
    
    while (cur < end && isspace(*cur))
        cur++;
    
    // set HTTP request headers.
    while (cur < end)
    {
        beg = cur;
        while (cur < end && (*cur) != '\n')
            cur++;
        http_request.add_header(beg, cur);
    
        while (cur < end && isspace(*cur))
            cur++;
    }
    
    cout << "method: " << http_request.get_method() << std::endl;
    assert(http_request.get_method() == HttpRequest::METHOD_GET);
    
    cout << "path: " << http_request.get_path() << std::endl;
    assert(http_request.get_path() == "/color.cc");
    
    cout << "version: " << http_request.get_version() << std::endl;
    assert(http_request.get_version() == HttpRequest::HTTPV_11);
    
    cout << "Connection: " << http_request.get_header("Connection") << std::endl;
    assert(http_request.get_header("Connection") == "keep-alive");
    
    cout << "Pragma: " << http_request.get_header("Pragma") << std::endl;
    assert(http_request.get_header("Pragma") == "no-cache");
    
    cout << "Host: " << http_request.get_header("Host") << std::endl;
    assert(http_request.get_header("Host") == "127.0.0.1:8080");
}

void test_post()
{
    string buf =   "POST /color.cc HTTP/1.1  \r\n"
                    "Connection: keep-alive \r\n"
                    "Pragma: no-cache     \r\n"
                    "Cache-Control: no-cache \r\n"
                    "Host: 127.0.0.1:8080\r\n\r\n";
    
    HttpRequest http_request;
    
    
    char* beg = const_cast<char*>(buf.data());
    char* end = beg + buf.size();
    char* cur = beg;
    
    // set HTTP request line.
    while (cur < end && !isspace(*cur))
        cur++;
    http_request.set_method(beg, cur);
    
    while (cur < end && isspace(*cur))
        cur++;
    
    beg = cur;
    while (cur < end && !isspace(*cur))
        cur++;
    http_request.set_path(beg, cur);
    
    while (cur < end && isspace(*cur))
        cur++;
    
    beg = cur;
    while (cur < end && !isspace(*cur))
        cur++;
    http_request.set_version(beg, cur);
    
    while (cur < end && isspace(*cur))
        cur++;
    
    // set HTTP request headers.
    while (cur < end)
    {
        beg = cur;
        while (cur < end && (*cur) != '\n')
            cur++;
        http_request.add_header(beg, cur);
    
        while (cur < end && isspace(*cur))
            cur++;
    }
    
    cout << "method: " << http_request.get_method() << std::endl;
    assert(http_request.get_method() == HttpRequest::METHOD_POST);
    
    cout << "path: " << http_request.get_path() << std::endl;
    assert(http_request.get_path() == "/color.cc");
    
    cout << "version: " << http_request.get_version() << std::endl;
    assert(http_request.get_version() == HttpRequest::HTTPV_11);
    
    cout << "Connection: " << http_request.get_header("Connection") << std::endl;
    assert(http_request.get_header("Connection") == "keep-alive");
    
    cout << "Pragma: " << http_request.get_header("Pragma") << std::endl;
    assert(http_request.get_header("Pragma") == "no-cache");
    
    cout << "Host: " << http_request.get_header("Host") << std::endl;
    assert(http_request.get_header("Host") == "127.0.0.1:8080");
}

int main()
{
    // run test cases.
    test_get();
    test_post();
    
    cout << ">>> ALL PASSED..." << endl;
    return 0;
}
