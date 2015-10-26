//
// Date: 2015/10/24
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_HTTPAUTHORIZATION_H
#define MINIHTTPD_HTTPAUTHORIZATION_H

#include "../Noncopyable.h"
#include "../TrieTree.h"
#include "../Base64.h"
#include "../Logger.h"
#include "../md5/md5.h"

#include <ctype.h>
#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::endl;

// url字符集
vector<char> charset =
{
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 
    'h', 'i', 'j', 'k', 'l', 'm', 'n', 
    'o', 'p', 'q', 'r', 's', 't', 'u', 
    'v', 'w', 'x', 'y', 'z', 'A', 'B',
    'C', 'D', 'E', 'F', 'G', 'H', 'I',
    'J', 'K', 'L', 'M', 'N', 'U', 'V',
    'W', 'X', 'Y', 'Z', '0', '1', '2',
    '3', '4', '5', '6', '7', '8', '9',
    '*', '-', '_', '+', '.', '/'
};

//
// 用户-密码组合
//
class UserPasswd
{
    public:
        UserPasswd(const string& p, const string& u, const string& pwd):
            path(p),
            user(u),
            passwd(pwd)
        {
        }
        
        UserPasswd(const UserPasswd&) = default;
        UserPasswd& operator=(const UserPasswd&) = default;
        
        string path;
        string user;
        string passwd;
};

//
// 支持Http认证
//
class HttpAuthorization : public NonCopyable
{
    public:
        // 支持基本认证，摘要认证
        enum class AuthMethod {basic, digest};
        
        HttpAuthorization(AuthMethod m):
            _auth_method(m),
            _trie(charset),
            _method_str_map({"Basic", "Digest"})
        {
        }
        
        //
        // 为一个目录path添加认证规则(user,password)
        //
        void add(const string& path, const string& user, const string& passwd)
        {
            UserPasswd user_passwd(path, user, passwd);
            _trie.insert(path, user_passwd);
        }
        
        //
        // 查找一个文件是否需要认证
        // 如果不需要返回空指针，否则返回指向UserPasswd结构的指针
        //
        UserPasswd* check(const string& path)
        {
            return _trie.find(path);
        }
        
        //
        // 认证名字string
        //
        string auth_name()
        {
            return _method_str_map[static_cast<int>(_auth_method)];
        }
        
        //
        // 返回认证方法
        //
        AuthMethod auth_method()
        {
            return _auth_method;
        }
        
        //
        // 认证
        //
        virtual bool auth(const string& http_method, const string&, const UserPasswd&) = 0;
        
    protected:
        AuthMethod _auth_method;
        TrieTree<UserPasswd> _trie;
        const vector<string> _method_str_map;
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
        bool auth(const string& http_method, const string& auth_field, const UserPasswd& user_passwd)
        {
            string auth_message;
            
            auto beg = auth_field.begin();
            auto end = auth_field.end();
            
            // skip over space.
            while (beg < end && isspace(*beg))
                beg++;
            
            // auth method
            while (beg < end && !isspace(*beg))
                beg++;
            
            // skip over space.
            while (beg < end && isspace(*beg))
                beg++;
            
            // auth message
            while (beg < end && !isspace(*beg))
                auth_message.push_back(*beg++);
            
            string userpasswd;
            Base64 base64;
            userpasswd = base64.decode(auth_message);
            
            auto it = userpasswd.begin();
            while (*it != ':')
                it++;
            
            string user(userpasswd.begin(), it);
            string passwd(it+1, userpasswd.end());
            
            return user_passwd.user == user &&
                user_passwd.passwd == passwd;
        }
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
        bool auth(const string& http_method, const string& auth_field, const UserPasswd& user_passwd)
        {
            string user;
            string realm;
            string nonce;
            string uri;
            string qop;
            string nc;
            string cnonce;
            string response;
            
            extract_auth_field(
                auth_field, user, realm, nonce, uri, qop, nc, cnonce, response);
            
            // 用户名不同直接认证失败
            if (user_passwd.user != user)
                return false;
            
            // 服务器生成摘要
            string response2 = digest(
                http_method, user, user_passwd.passwd, realm, nonce, uri, qop, nc, cnonce);

            return response == response2;
        }
        
        //
        // 生成摘要
        //
        string digest(const string& method, const string& user, 
            const string& passwd, const string& realm, 
            const string& nonce, const string& uri,
            const string& qop, const string& nc, const string& cnonce)
        {
            string A1 = user + ":" + realm + ":" + passwd;
            string A2 = method + ":" + uri;
            
            string HA1 = MD5(A1).toStr();
            string HA2 = MD5(A2).toStr();
            
            string source = HA1 + ":" + nonce + ":" + nc + ":" + cnonce + ":" + qop + ":" + HA2;
            return MD5(source).toStr();
        }
        
        //
        // 从Authorization中获取各项值
        //
        void extract_auth_field(const string& auth_field, string& user, string& realm, 
            string& nonce, string& uri, string& qop, string& nc, string& cnonce, string& response)
        {
            auto beg = auth_field.begin();
            auto end = auth_field.end();
            
            // skip over space.
            for (; beg < end && isspace(*beg); beg++);
            
            // auth method
            for (; beg < end && !isspace(*beg); beg++);
            
            // skip over space.
            for (; beg < end && isspace(*beg); beg++);
            
            // username
            int count = 0;
            for (; beg < end && !isspace(*beg) && *beg != '='; beg++);
            beg++;
            for (; beg < end && count < 2; beg++)
                if (*beg == '"')
                    count++;
                else
                    user.push_back(*beg);
            
            // skip over space.
            for (; beg < end && (isspace(*beg) || *beg == ','); beg++);
            
            // realm
            count = 0;
            for (; beg < end && !isspace(*beg) && *beg != '='; beg++);
            beg++;
            for (; beg < end && count < 2; beg++)
                if (*beg == '"')
                    count++;
                else
                    realm.push_back(*beg);
            
            // skip over space.
            for (; beg < end && (isspace(*beg) || *beg == ','); beg++);
            
            // nonce
            count = 0;
            for (; beg < end && !isspace(*beg) && *beg != '='; beg++);
            beg++;
            for (; beg < end && count < 2; beg++)
                if (*beg == '"')
                    count++;
                else
                    nonce.push_back(*beg);
            
            // skip over space.
            for (; beg < end && (isspace(*beg) || *beg == ','); beg++);
            
            // uri
            count = 0;
            for (; beg < end && !isspace(*beg) && *beg != '='; beg++);
            beg++;
            for (; beg < end && count < 2; beg++)
                if (*beg == '"')
                    count++;
                else
                    uri.push_back(*beg);
            
            // skip over space.
            for (; beg < end && (isspace(*beg) || *beg == ','); beg++);
            
            // qop
            for (; beg < end && !isspace(*beg) && *beg != '='; beg++);
            beg++;
            for (; beg < end && !isspace(*beg) && *beg != ','; beg++)
                    qop.push_back(*beg);
            
            // skip over space.
            for (; beg < end && (isspace(*beg) || *beg == ','); beg++);
            
            // nc
            for (; beg < end && !isspace(*beg) && *beg != '='; beg++);
            beg++;
            for (; beg < end && !isspace(*beg) && *beg != ','; beg++)
                    nc.push_back(*beg);
            
            // skip over space.
            for (; beg < end && (isspace(*beg) || *beg == ','); beg++);
            
            // cnonce
            count = 0;
            for (; beg < end && !isspace(*beg) && *beg != '='; beg++);
            beg++;
            for (; beg < end && count < 2; beg++)
                if (*beg == '"')
                    count++;
                else
                    cnonce.push_back(*beg);
            
            // skip over space.
            for (; beg < end && (isspace(*beg) || *beg == ','); beg++);
            
            // response
            count = 0;
            for (; beg < end && !isspace(*beg) && *beg != '='; beg++);
            beg++;
            for (; beg < end && count < 2; beg++)
                if (*beg == '"')
                    count++;
                else
                    response.push_back(*beg);
        }
};
#endif
