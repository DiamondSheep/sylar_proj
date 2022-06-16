#ifndef __LOG_H__
#define __LOG_H__
#include <stdint.h>
#include <string>
#include <memory>
#include <list>

namespace sylar{

class LogLevel{
public:
    enum Level { 
    // To show different information
        ALL   = 0,
        DEBUG = 1,
        INFO  = 2,
        WARN  = 3,
        ERROR = 4,
        FATAL = 5,
        OFF   = 6
    };
};

class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent ();
private:
    const char* m_filename = nullptr;
    int32_t m_line = 0;
    uint32_t m_threadID = 0;
    uint32_t m_fiberID = 0;
    uint32_t m_time;
    std::string m_content;
};

class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    std::string format(LogEvent::ptr event);
private:
};

class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender() {} // free space of derived class
    void log (LogLevel::Level level, LogEvent::ptr event);
private:
    LogLevel::Level m_level;
};

class Logger {
public:
    Logger(const std::string& LogName = "root");
    
    void addAppender (LogAppender::ptr appender);
    void delAppender (LogAppender::ptr appender);
    
    LogLevel::Level getLevel () const { return m_level; }
    void setLevel (LogLevel::Level level) { m_level = level; }

    void log (LogLevel::Level level, LogEvent::ptr event);
    // Other log functions
    // TODO
private:
    std::string m_logname;
    LogLevel::Level m_level;
    std::list<LogAppender::ptr> m_appenders;
};

/*
 * ---------------------------------------------
 *               Derived classes
 * ---------------------------------------------
*/
// For consoler
class StdoutLogAppender : public LogAppender {};

}

#endif