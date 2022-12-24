#include <iostream>
#include "../sylar/log.h"
#include "../sylar/util.h"

int main(int argc, char** argv)
{
    // 这样就可以输出日志了
    sylar::Logger::ptr logger(new sylar::Logger);
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));

    sylar::FileLogAppender::ptr file_appender(new sylar::FileLogAppender("./log.txt"));
    sylar::LogFormatter::ptr fmt(new sylar::LogFormatter("%d%T%p%T%m%n"));  // 自定义format
    file_appender->setFormatter(fmt);
    file_appender->setLevel(sylar::LogLevel::ERROR);    // 只输出error级别的日志到log.txt

    logger->addAppender(file_appender);

    // 0,1,2表示线程id
    // sylar::LogEvent::ptr event(new sylar::LogEvent(__FILE__, __LINE__, 0, sylar::GetThreadId(), sylar::GetFiberId(), time(0)));
    // event->getSS() << "日志内容";
    // logger->log(sylar::LogLevel::DEBUG, event);

    std::cout << "hello sylar log" << std::endl;

    SYLAR_LOG_INFO(logger) << "test macro";
    SYLAR_LOG_ERROR(logger) << "test macro error";

    SYLAR_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");
    // SYLAR_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aaaaa");

    auto l = sylar::LoggerMgr::GetInstance()->getLogger("xx");
    SYLAR_LOG_ERROR(l) << "xxx";

    return 0;
}