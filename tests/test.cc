#include <iostream>
#include "../sylar/log.h"

int main(int argc, char** argv)
{
    // 这样就可以输出日志了
    sylar::Logger::ptr logger(new sylar::Logger);
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));

    // 0,1,2表示线程id
    sylar::LogEvent::ptr event(new sylar::LogEvent(__FILE__, __LINE__, 0, 1, 2, time(0)));

    logger->log(sylar::LogLevel::DEBUG, event);

    std::cout << "hello sylar log" << std::endl;

    return 0;
}