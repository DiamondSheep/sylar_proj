#include "threads.hpp"
#include "utils.hpp"
#include "log.hpp"
#include <time.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

int count = 0;

sylar::Mutex_RW s_mutex_wr;
sylar::Mutex s_mutex;

void fun1() {
    SYLAR_LOG_INFO(g_logger) << "thread name: " 
                             << sylar::Thread::GetName() 
                             << " this.name: " 
                             << sylar::Thread::GetThis()->getName()
                             << " thread id: "
                             << sylar::GetThreadID()
                             << " this.id: " 
                             << sylar::Thread::GetThis()->getID();
    for (int i = 0 ; i < 10000; ++i) {
        //sylar::Mutex_RW::WriteLock lock(s_mutex_wr);
        sylar::Mutex::Lock lock(s_mutex);
        ++count;
    }
}

void fun2() {
    int count = 1;
    while(count){
        SYLAR_LOG_INFO(g_logger) << "==============================================================";
        count --;
    }
}

void fun3() {
    int count = 1;
    while(count) {
        SYLAR_LOG_INFO(g_logger) << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
        count --;
    }
}

int main (int argc, char* argv[]) {
    SYLAR_LOG_INFO(g_logger) << "thread test begin";
    g_logger->clearAppender();
    g_logger->addAppender(sylar::LogAppender::ptr(new sylar::FileLogAppender("../data/log.txt")));
    clock_t start, end;
    std::vector<sylar::Thread::ptr> thrs;
    start = clock(); // time of starting
    for (int i = 0; i < 4; ++i) {
        sylar::Thread::ptr thr(new sylar::Thread(&fun2, "name_" + std::to_string(i*2)));
        sylar::Thread::ptr thr_2(new sylar::Thread(&fun3, "name_" + std::to_string(i*2+1)));
        thrs.push_back(thr);
        thrs.push_back(thr_2);
    }
    //sleep(1);
    for (int i = 0; i < thrs.size(); ++i) { 
        thrs[i]->join();
    }
    end = clock(); // time of the end;
    SYLAR_LOG_INFO(g_logger) << "cout: " << count;
    g_logger->clearAppender();
    g_logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender()));
    SYLAR_LOG_INFO(g_logger) << "thread test end";
    SYLAR_LOG_INFO(g_logger) << "running time: " << double(end-start)/CLOCKS_PER_SEC <<"s";
    return 0;
}