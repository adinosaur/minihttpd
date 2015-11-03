//
// Date: 2015/10/09
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_HTTPBASE_H
#define MINIHTTPD_HTTPBASE_H

#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::map;

class HttpBase
{
    public:
        // root dir.
        static string HTTP_ROOT_DIR;
        
        // http method.
        enum class HttpMethod 
        { unknown, get, head, put, post };
        
        // http version.
        enum class HttpVersion 
        { unknown, v09, v10, v11, v20 };
        
        // http status code
        enum class HttpStatusCode 
        { 
            unknown = 0, 
            s200 = 200, 
            s301 = 301, 
            s400 = 400, 
            s401 = 401,
            s404 = 404, 
            s500 = 500, 
            s501 = 501 
        };
        
        // map
        static map<HttpMethod, string> method_to_str_map;
        static map<string, HttpMethod> str_to_method_map;
        
        static map<HttpVersion, string> version_to_str_map;
        static map<string, HttpVersion> str_to_version_map;
        
        static map<HttpStatusCode, string> status_to_str_map;
        
        static map<string, string> content_type_map;
        static vector<char> url_charset;
        
        // set root dir.
        static void set_root_dir(const string& dir)
        {
            HTTP_ROOT_DIR = dir;
        }
};

#endif
