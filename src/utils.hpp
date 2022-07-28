#ifndef __UTILS_H__
#define __UTILS_H__

#include <unistd.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <execinfo.h>
#include <vector>
#include <string>
#include <sstream>


namespace sylar {

pid_t GetThreadID();
uint32_t GetFiberID();

void BackTrace(std::vector<std::string>& bt, int size, int skip);

std::string BackTraceToString(int size, int skip, const std::string& prefix = "");

}

#endif