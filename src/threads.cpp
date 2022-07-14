#include "threads.hpp"

namespace sylar {

// In my opinion, it takes these two variables as the member variables of the whole module.
static thread_local Thread* t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOWN";
static Logger::ptr g_logger = SYLAR_LOG_NAME("system");

Thread* Thread::GetThis() {
    return t_thread;
}

const std::string& Thread::GetName() {
    return t_thread_name;
}

void Thread::SetName(const std::string& name) {
    if (name.empty()) {
        return;
    }
    if (t_thread) {
        t_thread->m_name = name;
    }
    t_thread_name = name;
}

Thread::Thread(std::function<void()> callback, const std::string& name)
: m_callback(callback), m_name(name) {
    if (name.empty()) {
        m_name = "UNKNOWN";
    }
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (rt) {
        SYLAR_LOG_ERROR(g_logger) << "pthread_create thread fail, rt=" << rt << " name=" << m_name;
        throw std::logic_error("pthread_create error");
    }
    m_thread = 0;
}

Thread::~Thread() {
    if (m_thread) {
        pthread_detach(m_thread);
        // detach or join
    }
}

void Thread::join() {
    if(m_thread) {
        // Not finish yet
        int rt = pthread_join(m_thread, nullptr);
        if(rt) {
            SYLAR_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" << rt << " name=" << m_name;
            throw std::logic_error("pthread_join error");
        }
        m_thread = 0;
    }
}

void* Thread::run (void* arg) {
    Thread* thread = (Thread*) arg;
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = GetThreadID();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0,15).c_str());

    std::function<void()> callback;
    callback.swap(thread->m_callback); // for references free up
    
    callback();
    return 0;
}

}