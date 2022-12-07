#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <string>
// 各种类型int的头文件
#include <stdint.h>
// shared_ptr的头文件
#include <memory>

namespace sylar
{

// 日志事件
class LogEvent
{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();

private:
    const char* m_file = nullptr;   // 文件名
    int32_t m_line = 0;             // 行号
    uint32_t m_elapse = 0;          // 程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;        // 线程id
    uint32_t m_fiberId = 0;         // 协程id
    uint64_t m_time;                // 时间戳
    std::string m_content;          // 消息
};

// 日志级别
class LogLevel
{
public:
    enum Level {
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
};

// 日志格式器
class LogFormatter
{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    
    std::string format(LogEvent::ptr event);
private:
};

// 日志输出地
// 由于有多种日志输出地，此类作为它们的基类
class LogAppender
{
public:
    typedef std::shared_ptr<LogAppender> ptr;

    /**
     * @brief 析构函数
     * 定义为虚函数，是因为会有继承关系且该类多基类，详细见《effective C++》条款07
     */
    virtual ~LogAppender() {};  

    void log(LogLevel::Level level, LogEvent::ptr event);

private:
    LogLevel::Level m_level;    // 日志级别
};

// 日志器
class Logger
{
public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);
private:
    std::string m_name;
    LogLevel::Level m_level;
    LogAppender::ptr;
};

// 定义输出到控制台的Appender
class StdoutLogAppender : public LogAppender
{
};

// 定义输出到文件的Appender
class FileLogAppender : public LogAppender
{
};


}


#endif