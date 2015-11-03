//
// Date: 2015/10/24
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_HTTPAUTHORIZATION_H
#define MINIHTTPD_HTTPAUTHORIZATION_H

#include "../Base/Noncopyable.h"
#include "../Base/TrieTree.h"
#include "HttpBase.h"
#include <string>
#include <vector>

//
// 用户-密码组合
//
class UserPasswd
{
    public:
        UserPasswd(const std::string& p, const std::string& u, const std::string& pwd):
            path(p),
            user(u),
            passwd(pwd)
        {
        }
        
        UserPasswd(const UserPasswd&) = default;
        UserPasswd& operator=(const UserPasswd&) = default;
        
        std::string path;
        std::string user;
        std::string passwd;
};

//
// 支持Http认证
//
class HttpAuthorization : public NonCopyable
{
    public:
        // 支持基本认证，摘要认证
        enum class AuthMethod {basic, digest};
        
        HttpAuthorization(AuthMethod m);
        
        //
        // 为一个目录path添加认证规则(user,password)
        //
        void add(const std::string& path, const std::string& user, const std::string& passwd);
        
        //
        // 查找一个文件是否需要认证
        // 如果不需要返回空指针，否则返回指向UserPasswd结构的指针
        //
        UserPasswd* check(const std::string& path);
        
        //
        // 认证名字string
        //
        std::string auth_name();
        
        //
        // 返回认证方法
        //
        AuthMethod auth_method();
        
        //
        // 认证
        //
        virtual bool auth(const std::string& http_method, const std::string&, const UserPasswd&) = 0;
        
    protected:
        AuthMethod _auth_method;
        TrieTree<UserPasswd> _trie;
        const std::vector<std::string> _method_str_map;
};

//
// 基本认证
//
class BasicAuth : public HttpAuthorization
{
    public:
        BasicAuth():
            HttpAuthorization(AuthMethod::basic)
        {
        }
        
        //
        // 认证
        //
        bool auth(const std::string& http_method, const std::string& auth_field, const UserPasswd& user_passwd);
};

//
// 摘要认证
//
class DigestAuth : public HttpAuthorization
{
    public:
        DigestAuth():
            HttpAuthorization(AuthMethod::digest)
        {
        }
        
        //
        // 认证
        //
        bool auth(const std::string& http_method, const std::string& auth_field, const UserPasswd& user_passwd);
        
        //
        // 生成摘要
        //
        std::string digest(const std::string& method, const std::string& user, 
            const std::string& passwd, const std::string& realm, 
            const std::string& nonce, const std::string& uri,
            const std::string& qop, const std::string& nc, const std::string& cnonce);
        
        //
        // 从Authorization中获取各项值
        //
        void extract_auth_field(const std::string& auth_field, std::string& user, std::string& realm, 
            std::string& nonce, std::string& uri, std::string& qop, 
            std::string& nc, std::string& cnonce, std::string& response);
};
#endif
