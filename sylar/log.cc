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

LogFormatter::LogFormatter(const std::string& pattern) 
    :m_pattern(pattern) {

}

std::string LogFormatter::format(LogEvent::ptr event) {
    std::stringstream ss;
    for (auto& i : m_items) {
        i->format(ss, event);   // 输出事件到接收流
    }
    return ss.str();
}

// 做日志格式解析
/** 仿照log4j格式：
 * 如果使用pattern布局就要指定的打印信息的具体格式ConversionPattern，打印参数如下：
 * %m 输出代码中指定的消息；
 * %M 输出打印该条日志的方法名；
 * %p 输出优先级，即DEBUG，INFO，WARN，ERROR，FATAL；
 * %r 输出自应用启动到输出该log信息耗费的毫秒数；
 * %c 输出所属的类目，通常就是所在类的全名；
 * %t 输出产生该日志事件的线程名；
 * %n 输出一个回车换行符，Windows平台为"rn”，Unix平台为"n”；
 * %d 输出日志时间点的日期或时间，默认格式为ISO8601，也可以在其后指定格式，比如：%d{yyyy-MM-dd HH:mm:ss,SSS}，输出类似：2002-10-18 22:10:28,921；
 * %l 输出日志事件的发生位置，及在代码中的行数；
 * [QC]是log信息的开头，可以为任意字符，一般为项目简称。
**/
// 格式可能为 %xxx 和 %xxx{xxx}，但也可能就是要输出 %，即 %%，则需要判断
void LogFormatter::init();        
    std::vector<std::pair<std::string, int>> vec;
    size_t last_pos = 0;        // 上一次解析位置
    for (size_t i = 0; i < m_pattern.size(); ++i) {

    }


}


}