#include "../include/Base/Logger.h"
#include <thread>

using namespace std;

void write_logging()
{
    thread log_thread(Logger::thread_func);
    log_thread.detach();
}

void trace()
{
    while (1)
    {
        auto trace_log = Logger::instance(Logger::TRACE);
        trace_log->logging(__FILE__, __LINE__, "TRACE-TEST", "OK");
    }
}

void debug()
{
    while (1)
    {
        auto debug_log = Logger::instance(Logger::DEBUG);
        debug_log->logging(__FILE__, __LINE__, "DEBUG-TEST", "OK");
    }
}

void info()
{
    while (1)
    {
        auto info_log = Logger::instance(Logger::INFO);
        info_log->logging(__FILE__, __LINE__, "INFO-TEST", "OK");
    }
}

void warn()
{
    while (1)
    {
        auto warn_log = Logger::instance(Logger::WARN);
        warn_log->logging(__FILE__, __LINE__, "WARN-TEST", "OK");
    }
}

void error()
{
    while (1)
    {
        auto error_log = Logger::instance(Logger::ERROR);
        error_log->logging(__FILE__, __LINE__, "ERROR-TEST", "OK");
    }
}

void trace_test()
{
    thread t(trace);
    t.detach();
}

void debug_test()
{
    thread t(debug);
    t.detach();
}

void info_test()
{
    thread t(info);
    t.detach();
}

void warn_test()
{
    thread t(warn);
    t.detach();
}

void error_test()
{
    thread t(error);
    t.detach();
}

void test()
{
    write_logging();
    
    trace_test();
    debug_test();
    info_test();
    warn_test();
    error_test();
}

int main(int ac, char** av)
{
    test();
    
    while (1)
        pause();
    return 0;
}
