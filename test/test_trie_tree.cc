
#include "../include/TrieTree.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

class UserPasswd
{
    public:
    UserPasswd(const string& u, const string& p):
        user(u),
        passwd(p)
    {
    }
    
    UserPasswd(const UserPasswd&) = default;
    UserPasswd& operator=(const UserPasswd&) = default;
    
    string user;
    string passwd;
};

int main()
{
    vector<char> set = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 
        'h', 'i', 'j', 'k', 'l', 'm', 'n', 
        'o', 'p', 'q', 'r', 's', 't', 'u', 
        'v', 'w', 'x', 'y', 'z', '.', '/'};
    
    TrieTree<UserPasswd> trie(set);
    
    UserPasswd userpasswd1("dinosaur", "12345");
    UserPasswd userpasswd2("wuman", "54321");
    
    string path1("/home/www");
    string path2("/home/projects");
    
    trie.insert(path1, userpasswd1);
    trie.insert(path2, userpasswd2);
    
    string file1("/home/www");
    string file2("/home/index.html");
    string file3("/home/www/index.html");
    
    UserPasswd* up1 = trie.find(file1);
    if (up1 != nullptr)
    {
        cout << "up1->user:" << up1->user << endl;
        cout << "up1->passwd:" << up1->passwd << endl;
    }
    
    UserPasswd* up2 = trie.find(file2);
    if (up2 != nullptr)
    {
        cout << "up2->user:" << up2->user << endl;
        cout << "up2->passwd:" << up2->passwd << endl;
    }
    
    UserPasswd* up3 = trie.find(file3);
    if (up3 != nullptr)
    {
        cout << "up2->user:" << up3->user << endl;
        cout << "up2->passwd:" << up3->passwd << endl;
    }

    return 0;
}
