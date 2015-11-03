#include "HttpAuthorization.h"

#include "../Base/Base64.h"
#include "../Base/Logger.h"
#include "../md5/md5.h"

#include <ctype.h>
#include <iostream>

using namespace std;

HttpAuthorization::HttpAuthorization(AuthMethod m):
    _auth_method(m),
    _trie(HttpBase::url_charset),
    _method_str_map({"Basic", "Digest"})
{
}

//
// 为一个目录path添加认证规则(user,password)
//
void HttpAuthorization::add(const string& path, const string& user, const string& passwd)
{
    UserPasswd user_passwd(path, user, passwd);
    _trie.insert(path, user_passwd);
}

//
// 查找一个文件是否需要认证
// 如果不需要返回空指针，否则返回指向UserPasswd结构的指针
//
UserPasswd* HttpAuthorization::check(const string& path)
{
    return _trie.find(path);
}

//
// 认证名字string
//
string HttpAuthorization::auth_name()
{
    return _method_str_map[static_cast<int>(_auth_method)];
}

//
// 返回认证方法
//
HttpAuthorization::AuthMethod HttpAuthorization::auth_method()
{
    return _auth_method;
}

//
// 认证
//
bool BasicAuth::auth(const string& http_method, const string& auth_field, const UserPasswd& user_passwd)
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

//
// 认证
//
bool DigestAuth::auth(const string& http_method, const string& auth_field, const UserPasswd& user_passwd)
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
string DigestAuth::digest(const string& method, const string& user, 
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
void DigestAuth::extract_auth_field(const string& auth_field, string& user, string& realm, 
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
