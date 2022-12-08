#include "log.h"

namespace sylar
{

Logger::Logger(const std::string& name)
    :m_name(name) {
}

// 添加appender
void addAppender(LogAppender::ptr appender) {
    m_appenders.push_back(appender);
}
// 删除appender
void delAppender(LogAppender::ptr appender) {
    // 遍历appenders集合，如果要删除的appender的指针在集合里，则删除
    for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it) {
        if (*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}

// 日志输出
void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
    // 传入的level大于等于m_level则输出
    // 遍历每个appender，再用appender把它输出出来
    if (level >= m_level) {
        for (auto& i : m_appenders) {
            // appenders集合里是每个appender的ptr
            i->log(level, event);   
        }
    }
}

void Logger::debug(LogEvent::ptr event) {
    log(LogLevel::DEBUG, event);
}

void Logger::info(LogEvent::ptr event) {
    log(LogLevel::INFO, event);
}

void Logger::warn(LogEvent::ptr event) {
    log(LogLevel::WARN, event);
}

void Logger::error(LogEvent::ptr event) {
    log(LogLevel::ERROR, event);
}

void Logger::fatal(LogEvent::ptr event) {
    log(LogLevel::FATAL, event);
}

FileLogAppender::FileLogAppender(const std::string& filename)
    :m_filename(filename) {

}

void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        m_filestream << m_formatter.format(event);
    }
}

// 有时候会重新打开日志文件，文件打开成功，返回true
bool FileLogAppender::reopen() {
    // 如果已经是打开的，则先关闭
    if (m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;
}

void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {
    // 初始化后有一个format，就可以直接把日志事件序列化下来
    if (level >= m_level) {
        std::cout << m_formatter.format(event);
    }
}


}