#include "util.h"

namespace sylar
{

pid_t GetThreadId() {   // 获取线程id
    return syscall(SYS_gettid);
}

uint32_t GetFiberId() { // 获取协程id，现在还没有实现协程，则默认返回0
    return 0;   
}

    
} // namespace sylar


    
