//
// Date: 2015/10/10
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_LOGGER_H
#define MINIHTTPD_LOGGER_H

#include "BlockingQueue.h"
#include "Helper.h"
#include <iostream>
#include <string>
#include <vector>

const int LEVEL_SIZE = 5;

//
// 日志类
//
class Logger
{
    public:
        enum {TRACE, DEBUG, INFO, WARN, ERROR};

        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        //
        // 创建对象实例
        //
        static Logger* instance(int level);
        
        //
        // 回收内存
        //
        static void destroy();
        
        //
        // 执行实际的写日志操作,在独立的线程中调用
        //
        static void thread_func();

        //
        // 添加至待记录日志队列
        //
        static void append(const string& log);

        //
        // 返回日志级别的字符串
        //
        const std::string& strLevel();

        //
        // 检测该日志是否高于当前该记录的日志级别
        // 并做日志记录
        //
        void logging(const string& file, int line, const string& content);
    private:
        // 
        // 构造函数为私有
        //
        Logger(int level):_level(level) {}
        
        //
        // 类静态变量
        //
        static char seperator;
        static int _curruct_level;
        static std::ostream& _output;
        
        static std::vector<std::string> _strLevelMap;
        static std::vector<Logger*> _instances; 
        static BlockingQueue<string> _queue;
        
        //
        // 类数据成员
        //
        int _level;
};
#endif
