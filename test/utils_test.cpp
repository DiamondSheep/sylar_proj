#include <cassert>
#include "utils.hpp"
#include "log.hpp"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void test_backtrace(){
    SYLAR_LOG_INFO(g_logger) << sylar::BackTraceToString(10, 0, "    ");
}

int main() {
    test_backtrace();
    return 0;
}