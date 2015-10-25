//
// Date: 2015/10/09
// Author: Dinosaur W.
//

#include "../include/HttpResponse.h"

#include <assert.h>

#include <iostream>
#include <string>

using namespace std;

void test()
{
    HttpResponse http_response;
    
    http_response.set_status_code(200);
    http_response.add_header("Host", "127.0.0.1:8080");
    http_response.add_header("Content-Type", "text/html");
    http_response.append_to_body("<HTML><TITLE>Index</TITLE>\r\n");
    http_response.append_to_body("<BODY><P>Hello World\r\n");
    http_response.append_to_body("/BODY></HTML>\r\n");
    
    string str = http_response.to_string();
    cout << str << endl;
    
    assert(str == "HTTP/1.0 200 OK\r\nHost: 127.0.0.1:8080\r\nContent-Type: text/html\r\n\r\n<HTML><TITLE>Index</TITLE>\r\n<BODY><P>Hello World\r\n/BODY></HTML>\r\n");
}

int main()
{
    // run test cases.
    test();
    
    cout << ">>> ALL PASSED..." << endl;
    return 0;
}
