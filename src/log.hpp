#ifndef __LOG_H__
#define __LOG_H__

// C libraries
#include <cstdint>
#include <time.h>
#include <stdarg.h>
#include <cstring>

// C++ libraries
#include <string>
#include <memory>
#include <list>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <functional>

#include "utils.hpp"
#include "singleton.hpp"

#define SYLAR_LOG_LEVEL(logger, level)\
    if (logger->getLevel() <= level)\
        sylar::LogEventWrap (sylar::LogEvent::ptr(new sylar::LogEvent (logger, level, __FILE__, __LINE__, 0, sylar::GetThreadID(), sylar::GetFiberID(), time(0)))).getSS()

#define SYLAR_LOG_ALL(logger)    SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ALL)
#define SYLAR_LOG_DEBUG(logger)  SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger)   SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger)   SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger)  SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
#define SYLAR_LOG_OFF(logger)    SYLAR_LOG_LEVEL(logger, sylar::LogLevel::OFF)

#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...)\
    if (logger->getLevel() <= level)\
        sylar::LogEventWrap(sylar::LogEvent::ptr(new sylar::LogEvent (logger, level, __FILE__, __LINE__, 0, sylar::GetThreadID(), sylar::GetFiberID(), time(0)))).getEvent()->format(fmt, __VA_ARGS__)

#define SYLAR_LOG_FMT_ALL(logger, fmt, ...)   SYLAR_LOG_FMT_LEVEL(logger, LogLevel::ALL, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, LogLevel::DEBUG, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...)  SYLAR_LOG_FMT_LEVEL(logger, LogLevel::INFO, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, LogLevel::WARN, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) SYLAR_LOG_FMT_LEVEL(logger, LogLevel::ERROR, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_OFF(logger, fmt, ...)   SYLAR_LOG_FMT_LEVEL(logger, LogLevel::OFF, fmt, __VA_ARGS__)

#define SYLAR_LOG_ROOT() sylar::SltLoggerMgr::GetInstance()->getRoot()
#define SYLAR_LOG_NAME(name) sylar::LoggerManager::GetInstance()->getLogger(name)

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
    // Convert to string
    static const char* ToString(const Level level);
};

class LogEvent{
public:
/*
 * This is the basic components for the Logger library
 * Initialization: a logger and other information 
 * 
 * 1. Feeding content to its stringstream by getSS()
 * 2. Log out by its logger ( event.getLogger()->log() )
 */
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent (std::shared_ptr<Logger> logger, 
              LogLevel::Level level,
              const char* file, 
              int32_t line, 
              uint32_t threadID, 
              uint32_t fiberID, 
              uint32_t elapse, 
              uint32_t time
              );

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

    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);
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

class LogEventWrap {
public:
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();
    std::stringstream& getSS();
    LogEvent::ptr getEvent();
private:
    LogEvent::ptr m_event;
};

class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern = ""); 
    void parse();
    std::string format(std::shared_ptr<Logger> logger_ptr, LogEvent::ptr event);

    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        FormatItem (const std::string& fmt = "") {}
        virtual ~FormatItem() {}
        virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) = 0;
    };
    bool isError() const { return m_error; }

private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    std::shared_ptr<Logger> m_logger_ptr;
    bool m_error = false;
};

class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;
    
    virtual ~LogAppender() {} // free space of derived class
    virtual void log (std::shared_ptr<Logger> logger_ptr, LogEvent::ptr event) = 0;
    
    void setFormatter (LogFormatter::ptr formatter) { m_formatter = formatter; }
    LogFormatter::ptr getFormatter () const { return m_formatter; }

protected:
    LogFormatter::ptr m_formatter;
};

// provide share_from_this() to generate a pointer of this
class Logger : public std::enable_shared_from_this<Logger> {
public:
    typedef std::shared_ptr<Logger> ptr;
    Logger(const std::string& LogName = "root");
    
    void addAppender (LogAppender::ptr appender);
    void delAppender (LogAppender::ptr appender);
    void clearAppender ();

    // Level control
    LogLevel::Level getLevel () const { return m_level; }
    void setLevel (LogLevel::Level level) { m_level = level; }
    LogFormatter::ptr getFormatter() const;
    void setFormatter (const LogFormatter::ptr formatter);
    void setFormatter (const std::string& str);

    // log 
    void log (LogLevel::Level level, LogEvent::ptr event);

    // Other log functions
    void debug (LogEvent::ptr event) { log (LogLevel::DEBUG, event); }
    void info (LogEvent::ptr event)  { log (LogLevel::INFO, event);  }
    void warn (LogEvent::ptr event)  { log (LogLevel::WARN, event);  }
    void error (LogEvent::ptr event) { log (LogLevel::ERROR, event); }
    
private:
    std::string m_logname;
    LogLevel::Level m_level; 
    std::list<LogAppender::ptr> m_appenders;
    LogFormatter::ptr m_formatter;
    //std::shared_ptr<Logger> m_root; // default logger
};

class LoggerManager {
public:
    LoggerManager ();
    std::shared_ptr<Logger> getLogger(const std::string& name);
    void addLogger (const std::string& name, std::shared_ptr<Logger> logger);
    void init();
    std::shared_ptr<Logger> getRoot() const { return m_root; }

private:
    std::map<std::string, std::shared_ptr<Logger> > m_loggers;
    std::shared_ptr<Logger> m_root;
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
    virtual void log (std::shared_ptr<Logger> logger_ptr, LogEvent::ptr event) override;
private:
};

class FileLogAppender : public LogAppender {
public:
    FileLogAppender (const std::string& filename);
    virtual void log (std::shared_ptr<Logger> logger_ptr, LogEvent::ptr event) override;
    bool reopen (); // reopen the file, return True if success
private:
    std::string m_filename;
    std::fstream m_filestream;
};

typedef Singleton<LoggerManager> SltLoggerMgr;

} // end of namespace
#endif