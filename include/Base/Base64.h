//
// Date: 2015/10/23
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_BASE64_H
#define MINIHTTPD_BASE64_H

#include <string>
#include <unordered_map>

using std::string;

class Base64
{
    public:
        string encode(const string plain);
        string encode(const char* beg, const char* end);
        
        string decode(const string cipher);
        string decode(const char* beg, const char* end);
        
    private:
        string do_decode(const char* beg);
        
        string do_encode1(const char* beg);
        string do_encode2(const char* beg);
        string do_encode3(const char* beg);
        
        static char _encode_map[64];
        static std::unordered_map<char, char> _decode_map;
};

#endif
