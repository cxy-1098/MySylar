#ifndef __SYLAR_LOG_H__
#define __SYLAR_LOG_H__

#include <string>
#include <stdint.h>     // 各种类型int的头文件
#include <memory>       // shared_ptr的头文件
#include <list> 
#include <stringstream> // 
#include <fstream>      // 文件的ofstream要用到

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
     * ////定义为虚函数，是因为会有继承关系且该类多基类，详细见《effective C++》条款07
     */
    virtual ~LogAppender() {};  

    // 定义基类的log，纯虚函数，所以在子类必须实现
    virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;

    // 不同的输出地有不同的输出格式
    void setFormatter(LogFormatter::ptr val) { m_formatter = val;}
    LogFormatter::ptr getFormatter() const { return m_formatter};
//// 因为是基类，成员属性用protected则子类就能使用到
protected:
    LogLevel::Level m_level;            // 日志级别
    LogFormatter::ptr m_formatter;      // 输出格式
};

// 日志器
class Logger
{
public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

    // 定义各种级别的日志输出器的函数
    // 在log.cc文件中实现
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);            // 添加appender
    void delAppender(LogAppender::ptr appender);            // 删除appender
    LogLevel::Level getLevel() const { return m_level;}     // 获取日志级别
    void setLevel(LogLevel::Level val) { m_level = val;}    // 设置级别
private:
    std::string m_name;                         // 日志名称
    LogLevel::Level m_level;                    // 日志级别
    std::list<LogAppender::ptr> m_appenders;    // Appender集合
};

// 定义输出到控制台的Appender
class StdoutLogAppender : public LogAppender
{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(LogLevel::Level level, LogEvent::ptr event) override;
private:
};

// 定义输出到文件的Appender
class FileLogAppender : public LogAppender
{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void log(LogLevel::Level level, LogEvent::ptr event) override;

    // 有时候会重新打开日志文件，文件打开成功，返回true
    bool reopen();              

private:
    std::string m_filename;         // 文件名
    std::ofstream m_filestream;     // ofstream是从内存到硬盘
};


}


#endif