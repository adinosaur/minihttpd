//
// Date: 2015/10/29
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_URLCHECK_H
#define MINIHTTPD_URLCHECK_H

#include <assert.h>
#include <string>
#include <stack>

//
// 配对自动机,状态转移表
//
//    * . / (*表示其它字符)
//  0|0 1 3
//  1|0 2 0
//  2|0 0 0
//  3|0 0 0
//
int Goto[4][3] = 
{
    {0, 1, 3},
    {0, 2, 0},
    {0, 0, 0},
    {0, 0, 0}
};

//
// 返回url的相对深度
// 根据状态机计算是否超出根目录
//
bool urlUnderRootDir(const std::string& url)
{
    int status = 0;
    std::stack<int> dir_stack;
    
    assert(url[0] == '/');
    
    for (int i = 1; i != url.size(); ++i)
    {
        switch (url[i])
        {
            case '.':
                status = Goto[status][1];
                break;
            case '/':
                status = Goto[status][2];
                break;
            default:
                status = Goto[status][0];
                break;
        }
        
        switch (status)
        {
            case 0:
            case 1:
                break;
            case 2:
                // 状态为2上升一层目录
                if (dir_stack.empty())
                    return false;
                else
                    dir_stack.pop();
                break;
            case 3:
                // 状态为3下降一层目录
                dir_stack.push(1);
                break;
        }
    }
    
    return true;
}

#endif
