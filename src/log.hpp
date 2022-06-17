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

    static const char* ToString(Level level);
};

class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent () {}

    const char* getFile() const { return m_filename; }
    int32_t getLine() const { return m_line; }
    uint32_t getThreadID() const { return m_threadID; }
    uint32_t getFiberID() const { return m_fiberID; }
    uint32_t getTime() const { return m_time; }
    const std::string& getContent() const { return m_content; } 
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
    LogFormatter(const std::string& pattern);
    void parse();
    //%t    %threadID %m%n
    std::string format(LogLevel::Level level, LogEvent::ptr event);

    class FormatItem{
    public:
         typedef std::shared_ptr<FormatItem> ptr;
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