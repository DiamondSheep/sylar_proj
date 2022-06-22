#ifndef __LOG_H__
#define __LOG_H__
#include <stdint.h>
#include <string>
#include <memory>
#include <list>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <utility>
#include <map>
#include <functional>
#include <time.h>
#include <cstring>
#include "utils.hpp"

#define SYLAR_LOG_LEVEL(logger, level)\
    if (logger->getLevel() <= level)\
        std::cout << sylar::LogEventWarp (sylar::LogEvent::ptr(new LogEvent (logger, level, __FILE__, __LINE__, 0, sylar::GetThreadID(), sylar::GetFiberID(), time(0)))).getSS().str()

#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
// TODO

namespace sylar{

class Logger;

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

    static const char* ToString(const Level level);
};

class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent (std::shared_ptr<Logger> logger, LogLevel::Level level,
              const char* file, int32_t line, 
              uint32_t threadID, uint32_t fiberID, 
              uint32_t elapse, uint32_t time);
    std::shared_ptr<Logger> getLogger() const { return m_logger; }
    LogLevel::Level getLevel() const { return m_level; }
    const char* getFileName() const { return m_filename; }
    int32_t getLineNumber() const { return m_line; }
    uint32_t getThreadID() const { return m_threadID; }
    uint32_t getFiberID() const { return m_fiberID; }
    uint32_t getElapse() const {return m_elapse; }
    uint32_t getTime() const { return m_time; }
    std::string getContent() const { return m_ss.str(); } 
    std::stringstream& getSS() { return m_ss; }
private:
    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
    const char* m_filename = nullptr;
    int32_t m_line = 0;
    uint32_t m_threadID = 0;
    uint32_t m_fiberID = 0;
    uint32_t m_elapse = 0;
    uint32_t m_time;
    std::stringstream m_ss;
};

class LogEventWarp {
public:
    LogEventWarp(LogEvent::ptr e);
    ~LogEventWarp();
    std::stringstream& getSS();
private:
    LogEvent::ptr m_event;
};

class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern = ""); //%d [%p{fmt}] %f{fmt} %l %m %n
    void parse();
    std::string format(LogLevel::Level level, LogEvent::ptr event);

    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        FormatItem (const std::string& fmt = "") {}
        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
};

class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;
    LogAppender (LogLevel::Level level=LogLevel::ALL) {
    }
    virtual ~LogAppender() {} // free space of derived class
    virtual void log (LogLevel::Level level, LogEvent::ptr event) = 0;
    
    void setFormatter (LogFormatter::ptr formatter) { m_formatter = formatter; }
    LogFormatter::ptr getFormatter () const { return m_formatter; }

protected:
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
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
    LogFormatter::ptr m_formatter;
};

/*
 * ---------------------------------------------
 *               Derived classes
 * ---------------------------------------------
*/
// For consoler
class StdoutLogAppender : public LogAppender {
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    virtual void log (LogLevel::Level, LogEvent::ptr event) override;
private:
};

class FileLogAppender : public LogAppender {
public:
    FileLogAppender (const std::string& filename);
    virtual void log (LogLevel::Level, LogEvent::ptr event) override;
    bool reopen (); // reopen the file, return True if success
private:
    std::string m_filename;
    std::fstream m_filestream;
};

} // end of namespace
#endif