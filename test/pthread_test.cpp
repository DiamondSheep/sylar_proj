#include "threads.hpp"
#include "utils.hpp"

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

void fun1() {
    SYLAR_LOG_INFO(g_logger) << "thread name: " 
                             << sylar::Thread::GetName() 
                             << " this.name: " 
                             << sylar::Thread::GetThis()->getName()
                             << " thread id: "
                             << sylar::GetThreadID()
                             << " this.id: " 
                             << sylar::Thread::GetThis()->getID();
}

void fun2() {

}

int main (int argc, char* argv[]) {
    SYLAR_LOG_INFO(g_logger) << "thread test begin";
    std::vector<sylar::Thread::ptr> thrs;
    for (int i = 0; i < 4; ++i) {
        sylar::Thread::ptr thr(new sylar::Thread(&fun1, "name_" + std::to_string(i)));
        thrs.push_back(thr);
    }
    //sleep(1);
    for (int i = 0; i < thrs.size(); ++i) { 
        thrs[i]->join();
    }
    SYLAR_LOG_INFO(g_logger) << "thread test end";
    return 0;
}