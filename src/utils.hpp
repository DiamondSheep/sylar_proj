#ifndef __UTILS_H__
#define __UTILS_H__

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

namespace sylar {

pid_t GetThreadID();

}

#endif