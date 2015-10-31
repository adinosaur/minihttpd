//
// Date: 2015/10/26
// Author: Dinosaur W.
//
// Date: 2015/10/31
// Author: Dinosaur W.
//
// depend on module: md5

#ifndef RANDOM_STRING_H
#define RANDOM_STRING_H

#include "md5/md5.h"
//#include "Base64.h"
#include <time.h>
#include <string>

//
// private key
//
std::string private_key("uvmMi12NRtYpEj9nUQ6AaHx5scPbJF4e");

//
// 字符集
//
char CharSet[] = 
{
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
    'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
    'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
    'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z'
};

//
// 获取一个随时间变化的随机字符串
// random-string = Hash(time-string ":" private-key)
//
std::string random_string(int second)
{
    std::string time_str;
    time_t cur_time = time(NULL);
    cur_time = (cur_time / second) * second;
   
    // 10进制转换为26+26+10进制
    while (cur_time > 0)
    {
        time_str.push_back(CharSet[cur_time % sizeof(CharSet)]);
        cur_time /= sizeof(CharSet);
    }

    return MD5(time_str + ":" + private_key).toStr();
}

#endif
