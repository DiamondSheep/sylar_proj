#include "utils.hpp"

namespace sylar {

pid_t GetThreadID() {
    return syscall(SYS_gettid);
}

}