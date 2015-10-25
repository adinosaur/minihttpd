//
// Date: 2015/10/24
// Author: Dinosaur W.
//

#ifndef TRIETREE_H
#define TRIETREE_H

#include <string>
#include <vector>
#include <unordered_map>

using std::string;
using std::vector;
using std::unordered_map;

//
// Trie树
//
// 用来支持Http协议的认证功能
// 给定一个资源的url，需要快速判断这个资源是否是在那些需要认证的目录之下
// 例如假设目录a需要认证，那么访问目录a之下的所有文件都需要认证
// 于是转换成需找字符串寻找前缀的问题
// 
// 用Trie树优点是多模式匹配速度快，但内存消耗大
//

template <typename T>
class TrieTreeNote
{
    public:
        TrieTreeNote(int child_size):
            child(child_size, nullptr)
        {
        }

        ~TrieTreeNote()
        {
            destroy();
        }
        
        void set_val(const T& v)
        {
            destroy();
            val = new T(v);
        }

        T* get_val()
        {
            return val;
        }

        void destroy()
        {
            if (val != nullptr)
                delete val;
        }
        
        bool val_is_null()
        {
            return val == nullptr;
        }
        
        vector<TrieTreeNote*> child;
    private:
        T* val = nullptr;
};

template <typename T>
class TrieTree
{
    public:
        TrieTree(const vector<char>& vec)
        {
            build_map(vec);
            root = new TrieTreeNote<T>(char_size());
        }
        
        ~TrieTree()
        {
            destroy(root);
        }
        
        //
        // 查找字符串
        //
        T* find(const string& key)
        {
            TrieTreeNote<T>* cur = root;
            int next;
            for (int i = 0; i != key.size(); ++i)
            {
                // 如果出现不在字符集_char_set的字符，返回fasle
                auto it = _char_map.find(key[i]);
                if (it == _char_map.end())
                    return nullptr;
                
                // 查找失败
                if (cur == nullptr)
                    return nullptr;
                
                // 查找成功
                if (!cur->val_is_null())
                    return cur->get_val();
                
                next = it->second;
                cur = cur->child[next];
            }
            
            return nullptr;
        }
        
        //
        // 添加字符串
        //
        bool insert(const string& key, const T& val)
        {
            TrieTreeNote<T>* cur = root;
            int next;
            for (int i = 0; i != key.size(); ++i)
            {
                // 如果出现不在字符集_char_set的字符，返回fasle
                auto it = _char_map.find(key[i]);
                if (it == _char_map.end())
                    return false;
                
                next = it->second;
                
                if (cur->child[next] == nullptr)
                    cur->child[next] = new TrieTreeNote<T>(char_size());

                if (i == key.size() - 1)
                    cur->set_val(val);

                cur = cur->child[next];
            }
            return true;
        }
        
        //
        // 返回字符集大小
        //
        int char_size()
        {
            return _char_map.size();
        }
        
    private:
        //
        // 构建字符到键值的映射表
        //
        void build_map(const vector<char>& vec)
        {
            for (int i = 0; i != vec.size(); ++i)
                _char_map[vec[i]] = i;
        }
        
        //
        // 销毁树
        //
        void destroy(TrieTreeNote<T>* root)
        {
            if (root == nullptr)
                return;
            
            for (auto c : root->child)
                destroy(c);
            
            delete root;
        }
        
        TrieTreeNote<T>* root = nullptr;
        unordered_map<char, int> _char_map;
};

#endif
