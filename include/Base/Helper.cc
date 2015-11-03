#include "Helper.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include <iostream>
#include <chrono>
#include <string>
#include <stack>


//
// 读取一行数据
//
int Helper::readline(int fd, char *buf, int size)
{
    int n, rc;
    char c, *ptr;
    
    ptr = buf;
    
    for (n = 0; n < size-1; ++n)
    {
        rc = ::read(fd, &c, 1);
        if (rc == 1)
        {
            if (c == '\n')
                break;
            *ptr++ = c;
        }
        else if (rc == 0)
        {
            break;
        }
        else
        {
            return -1;
        }
    }

    *ptr++ = '\n';
    *ptr = '\0';
    
    //std::cout << (ptr-buf) / sizeof(char*) << std::endl;
    return (ptr-buf);
}

//
// 当前日期字符串
//
string Helper::date()
{
    char buf[80];
    time_t rawtime;
    struct tm * timeinfo;
    
    ::time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buf, 80, "%Y%m%d", timeinfo);
    
    return buf;
}

//
// 当前时间字符串
//
string Helper::time()
{
    char buf[80];
    time_t rawtime;
    struct tm * timeinfo;
    
    ::time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buf, 80, "%H:%M:%S", timeinfo);
    
    return buf;
}

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
bool Helper::urlUnderRootDir(const string& url)
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
