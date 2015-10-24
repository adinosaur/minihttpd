//
// Date: 2015/10/24
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_HTTPAUTHORIZATION_H
#define MINIHTTPD_HTTPAUTHORIZATION_H

#include "NonCopyable.h"
#include "../TrieTree.h"

#include <string>
#include <map>

//
// 支持Http认证
//
class HttpAuthorization : public NonCopyable
{
    public:
        void add_();
    private:
        TrieTree<> _user_passwd_map;
        std::map<std::string, std::string> _user_passwd_map;
};

std::map<std::string, std::string> _user_passwd_map = 
{
    {"admin", "admin"}
};

#endif
