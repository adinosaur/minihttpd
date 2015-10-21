//
// Date: 2015/10/10
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_LOGGER_H
#define MINIHTTPD_LOGGER_H

#include <iostream>
#include <chrono>
#include <iomanip>
#include <string>
#include <time.h>

std::ostream& LOGGER = std::cerr;

enum LogLevel { 
    TRACE, 
    DEBUG, 
    INFO, 
    WARN, 
    ERROR 
};

class Logger
{
    public:
        
        Logger(LogLevel level, std::ostream& log = LOGGER):
            _level(level),
            _log(log)
        {
        }
        
        void set_level(LogLevel level)
        {
            _cur_log_level = level;
        }
        
        //
        // check level then logging
        //
        template <typename ... Args>
        void logging(const std::string& file, int line, const Args& ... args)
        {
            if (_level >= _cur_log_level)
            {
                _log << this->date() << SEP;
                _log << this->time() << SEP;
                _log << str_level() << SEP;
                _logging(args...);
                _log << file << ":" << line << SEP;
                _log << std::endl;
            }
        }
        
    private:
        //
        // 为了终止递归，定义一个非可变参数的_logging函数
        //
        template <typename T>
        void _logging(const T& t)
        {
            _log << t << SEP;
        }
        
        template <typename T, typename ... Args>
        void _logging(const T& t, const Args& ... args)
        {
            _log << t << SEP;
            _logging(args...);
        }
        
        std::string str_level()
        {
            switch (_level)
            {
                case TRACE:
                    return "TRACE";
                case DEBUG:
                    return "DEBUG";
                case INFO:
                    return "INFO";
                case WARN:
                    return "WARN";
                case ERROR:
                    return "ERROR";
            }
        }
        
        static std::string date()
        {
            char buf[80];
            time_t rawtime;
            struct tm * timeinfo;
            
            ::time(&rawtime);
            timeinfo = localtime(&rawtime);

            strftime(buf, 80, "%Y%m%d", timeinfo);
            
            return buf;
        }
        
        static std::string time()
        {
            char buf[80];
            time_t rawtime;
            struct tm * timeinfo;
            
            ::time(&rawtime);
            timeinfo = localtime(&rawtime);

            strftime(buf, 80, "%H:%M:%S", timeinfo);
            
            return buf;
        }
        
        static char SEP;
        
        // logging level, default is DEBUG
        static LogLevel _cur_log_level;
        
        LogLevel _level;
        std::ostream& _log;
};

char Logger::SEP = '|';

// default logging level.
// LogLevel Logger::_cur_log_level = DEBUG;
LogLevel Logger::_cur_log_level = TRACE;

// logger object.
Logger TRACE_LOG(TRACE);
Logger DEBUG_LOG(DEBUG);
Logger INFO_LOG(INFO);
Logger WARN_LOG(WARN);
Logger ERROR_LOG(ERROR);

#endif
