#ifndef __THREADS_H__
#define __THREADS_H__

#include <thread>
#include <functional>
#include "log.hpp"
#include "utils.hpp"

namespace sylar {

class Thread {
public:
    typedef std::shared_ptr<Thread> ptr;
    Thread(std::function<void()> callback, const std::string& name);
    ~Thread();

    pid_t getID() { return m_id; }
    const std::string& getName() const { return m_name; }
    void join();
    static Thread* GetThis();
    static const std::string& GetName(); // for logger 
    static void SetName(const std::string& name);
private:
    Thread(const Thread&) = delete;
    Thread(const Thread&&) = delete;
    Thread operator=(const Thread&) = delete;
    static void* run (void* arg);
    // member data
    pid_t m_id;
    pthread_t m_thread;
    std::function<void()> m_callback;
    std::string m_name;
};

}

#endif