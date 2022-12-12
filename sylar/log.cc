#include "log.h"
#include <map>

namespace sylar
{

const char* LogLevel::ToString(LogLevel::Level level) {
    switch(level) { // 定义了一个宏来取level
# define XX(name) \
    case LogLevel::name: \
        return #name;   \
        break;

    xx(DEBUG),
    XX(INFO),
    XX(WARN),
    XX(ERROR),
    XX(FATAL);
#undef XX
    default:
        return "UNKNOW";
    }
    return "UNKNOW";
}

// 七八个实例
// 输出信息
class MessageFormatItem : public LogFormatter::FormatItem 
{
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getContent();
    }
};

// 输出等级
class LevelFormatItem : public LogFormatter::FormatItem 
{
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << LogLevel::ToString(level);
    }
};

// 输出启动后的消耗时间
class ElapseFormatItem : public LogFormatter::FormatItem 
{
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getElapse();
    }
};

// 输出名称
class NameFormatItem : public LogFormatter::FormatItem 
{
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << logger->getName();
    }
};

// 输出线程号
class ThreadIdFormatItem : public LogFormatter::FormatItem 
{
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getThreadId();
    }
};

// 输出协程号
class FiberIdFormatItem : public LogFormatter::FormatItem 
{
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFiberId();
    }
};

// 输出时间
class DateTimeFormatItem : public LogFormatter::FormatItem 
{
public:
    DateTimeFormatItem(const std::string& format = "%Y:%m:%d %H:%M:%S")
        :m_format(format) {

    }
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getTime();
    }
private:
    std::string m_format;
};

// 输出文件名
class FilenameFormatItem : public LogFormatter::FormatItem 
{
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFile();
    }
};

// 输出行号
class LineFormatItem : public LogFormatter::FormatItem 
{
public:
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getLine();
    }
};

// 输出
class StringFormatItem : public LogFormatter::FormatItem 
{
public:
    StringFormatItem(const std::string& str) 
        :FormatItem(str), m_string(str) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << m_string;
    }
private:
    std::string m_string;
};

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

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        m_filestream << m_formatter.format(logger, level, event);
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

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    // 初始化后有一个format，就可以直接把日志事件序列化下来
    if (level >= m_level) {
        std::cout << m_formatter.format(logger, level, event);
    }

LogFormatter::LogFormatter(const std::string& pattern) 
    :m_pattern(pattern) {

}

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    std::stringstream ss;
    for (auto& i : m_items) {
        i->format(ss, logger, level, event);   // 输出事件到接收流
    }
    return ss.str();
}

// 做日志格式解析
// 格式可能为 %xxx(%后直接跟格式) 和 %xxx{xxx}(格式里还套了格式，第一个xxx格式称为str，第二个xxx称为format），
// 但也可能就是要输出 %，即转义 %%，则需要判断
void LogFormatter::init();        
    // str, format, type    
    // 如%xxx{xxx}，第一个xxx称为str格式，第二个xxx称为format格式，type 0表异常状态，1表正常状态
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr;               // normal string
    for (size_t i = 0; i < m_pattern.size(); ++i) {
        // 如果等于%，说明他可能是一种格式，但到底是不是还需要确定一下
        if (m_pattern[i] != '%') {    // 不等于%，则不是格式
            str.append(1, m_pattern[i]);
            continue;
        }

        // 说明是真的%，即%%，不是格式
        if ((i + 1) < m_pattern.size()) {
            if (m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
        }

        // 等于%，可能是格式
        size_t n = i + 1;       // 判断%下一个字符
        int fmt_status = 0;     // format状态，0初始状态，1开始解析格式，2解析完毕格式
        size_t fmt_begin = 0;   // %xxx{xxx} 中 {} 内的xxx的开始位置

        std::string str;
        std::string fmt;
        while (n < m_pattern.size()) {
            // 如果%后有空格不连续了，那就不是格式了
            if (isspace(m_pattern[n])) { 
                break;
            }
            // 始终以%xxx{xxx}为例子，第一个xxx称为str格式，第二个xxx称为format格式
            if (fmt_status == 0) {
                // 遇到了左括号，说明 %xxx{xxx} 的 { 之前的str xxx已经找到了，即格式已经找到了
                if (m_pattern[n] == '{') {  
                    str = m_pattern.substr(i + 1, n - i - 1);
                    fmt_status = 1;     // 开始解析格式
                    fmt_begin = n;      
                    ++n;
                    continue;
                }
                if (fmt_status == 1) {
                    // %xxx{xxx} 中 {} 内的format xxx 解析完毕
                    if (m_pattern[n] == '}') {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        fmt_status = 2; // 解析完毕格式
                        break;
                    }
                }
            }
        }

        // 循环完了之后的各种状态
        if (fmt_status == 0) {          // 只有第一个str格式的xxx，正常的情况
            if (!nstr.empty()) {        // 格式前面有normal string，也放进去
                vec.push_back(std::make_pair(nstr, "", 0));
            }
            str = m_pattern.substr(i + 1, n - i - 1);   
            vec.push_back(std::make_tuple(str, fmt, 1));    // 将解析的结果放入vector，1表正常状态
            i = n;
        }else if (fmt_status == 1) {    // 只找到 { 没有找到 } 括号，是错误的情况
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("pattern_error", fmt, 0));    // 0表异常状态
        }else if (fmt_status == 2) {    // 找到了 {} ，正常情况
            if (!nstr.empty()) {        // 格式前面有normal string，也放进去
                vec.push_back(std::make_pair(nstr, "", 0));
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n;
        }
    }

    if (!nstr.empty()) {        
        vec.push_back(std::make_pair(nstr, "", 0));
    }

    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>> s_format_items = {
#define XX(str, C) \
        {#str, [](const std:: string& fmt) { return FormatItem::ptr(new C(fmt));}}

        XX(m, MessageFormatItem),

    }

    /** 仿照log4j格式：
    * 如果使用pattern布局就要指定的打印信息的具体格式ConversionPattern，打印参数如下：
    * %m 输出代码中指定的消息；
    * %p 输出优先级level，即DEBUG，INFO，WARN，ERROR，FATAL；
    * %r 输出自应用启动到输出该log信息耗费的毫秒数；
    * %c 输出所属的类目，日志名称，通常就是所在类的全名；
    * %t 输出产生该日志事件的线程名id；
    * %n 输出一个回车换行符，Windows平台为"rn”，Unix平台为"n”；
    * %d 输出日志时间点的日期或时间，默认格式为ISO8601，也可以在其后指定格式，比如：%d{yyyy-MM-dd HH:mm:ss,SSS}，输出类似：2002-10-18 22:10:28,921；
    * %l 输出日志事件的发生位置，及在代码中的行数；
    * %f 输出文件名
    **/


}




}