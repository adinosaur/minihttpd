#include "Logger.h"
#include <assert.h>
#include <pthread.h>
#include <string>

using namespace std;

// 日志分隔符
char Logger::seperator = '|';

// 日志记录级别
int Logger::_curruct_level = TRACE;

// 日志输出文件
ostream& Logger::_output(cerr);

// instance数组
vector<Logger*> Logger::_instances(LEVEL_SIZE, nullptr);

// 阻塞队列
BlockingQueue<string> Logger::_queue;

// 映射表
vector<string> Logger::_strLevelMap = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR"};

Logger* Logger::instance(int level)
{
    assert(level >= 0 && level < LEVEL_SIZE);
    if (_instances[level] == nullptr)
        _instances[level] = new Logger(level);
    return _instances[level];
}

void Logger::destroy()
{
    for (auto p : _instances)
        delete p;
}

const string& Logger::strLevel()
{
    return _strLevelMap[_level];
}

void Logger::thread_func()
{
    while(1)
    {
        const string& msg = _queue.front();
        _output << msg << endl;
        _queue.pop_front();
    }
}

void Logger::append(const string& log)
{
    _queue.push_back(log);
}

void Logger::logging(const string& file, int line, const string& content)
{
    if (_level >= _curruct_level)
    {
        std::string msg;
        
        msg.append(Helper::date());
        msg.push_back(seperator);

        msg.append(Helper::time());
        msg.push_back(seperator);
        
        msg.append(std::to_string(pthread_self()));
        msg.push_back(seperator);
        
        msg.append(strLevel());
        msg.push_back(seperator);

        msg.append(content);
        
        msg.push_back(seperator);
        msg.append(file);
        msg.push_back(':');
        msg.append(std::to_string(line));
        msg.push_back(seperator);

        // 插入阻塞队列
        _queue.push_back(msg);
    }
}
