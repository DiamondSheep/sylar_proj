#include "log.hpp"

namespace sylar{
/*
 * --------------- LogLevel ---------------
*/
const char* LogLevel::ToString(const LogLevel::Level level) {
// Sylar write it with macro 
    switch (level) {
        case LogLevel::ALL:
            return "ALL"; break;
        case LogLevel::DEBUG:
            return "DEBUG"; break;
        case LogLevel::INFO:
            return "INFO"; break;
        case LogLevel::WARN:
            return "WARN"; break;
        case LogLevel::ERROR:
            return "ERROR"; break;
        case LogLevel::FATAL:
            return "FATAL"; break;
        case LogLevel::OFF:
            return "OFF"; break;
        default:
            return "UNKNOWN";
    }
    return "UNKNOWN";
}

/* 
 * --------------- LogEvent ---------------
 */
LogEvent::LogEvent (std::shared_ptr<Logger> logger,
                    LogLevel::Level level, 
                    const char* filename, 
                    int32_t line, 
                    uint32_t threadID, 
                    uint32_t fiberID, 
                    uint32_t elapse, 
                    uint32_t time)
                    :m_logger(logger),
                     m_level(level),
                     m_filename(filename), 
                     m_line(line), 
                     m_threadID(threadID), 
                     m_fiberID(fiberID), 
                     m_elapse(elapse), 
                     m_time(time) {
               }
void LogEvent::format(const char* fmt, ...){
    va_list al;
    va_start(al, fmt); // allow to visit 
    format(fmt, al);
    va_end(al);
}
void LogEvent::format(const char* fmt, va_list al){
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if (len != -1){
        m_ss << std::string (buf, len);
        free (buf);
    }
}

/* 
 * --------------- LogEventWrap ---------------
 */
LogEventWrap::LogEventWrap(LogEvent::ptr e)
: m_event(e){}

LogEventWrap::~LogEventWrap(){
    m_event->getLogger()->log(m_event->getLevel(), m_event);
}

std::stringstream& LogEventWrap::getSS() {
    return m_event->getSS();
}

LogEvent::ptr LogEventWrap::getEvent(){
    return m_event;
}

/* 
 * --------------- FormatItem ---------------
 */
class MessageFormatItem : public LogFormatter::FormatItem{
public:
    MessageFormatItem (const std::string& format = ""){ }
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem{
public:
    LevelFormatItem (const std::string& format = ""){ }
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem{
public:
    ElapseFormatItem (const std::string& format = ""){ }
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getElapse();
    }
};

// Note: Different from Sylar, we don't implement the getName() function

class ThreadIDFormatItem : public LogFormatter::FormatItem{
public:
    ThreadIDFormatItem (const std::string& format = ""){ }
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getThreadID();
    }
};

class FiberIDFormatItem : public LogFormatter::FormatItem{
public:
    FiberIDFormatItem (const std::string& format = ""){ }
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getFiberID();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem{
public:
    DateTimeFormatItem (const std::string& format)
    : m_format(format) {
        if (m_format.empty()){
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    std::string m_format;
};

class FileNameFormatItem : public LogFormatter::FormatItem{
public:
    FileNameFormatItem (const std::string& format = ""){ }
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getFileName();
    }
};

class LineNumberFormatItem : public LogFormatter::FormatItem{
public:
    LineNumberFormatItem (const std::string& format = ""){ }
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getLineNumber();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem{
public:
    NewLineFormatItem (const std::string& format = ""){ }
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << std::endl;
    }
};

class StringFormatItem : public LogFormatter::FormatItem{
public:
    StringFormatItem (const std::string& str)
    : FormatItem(str), m_string(str) {} // Initializing base class
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << m_string;
    }
private:
    std::string m_string;
};

class TabFormatItem : public LogFormatter::FormatItem{
public:
    TabFormatItem (const std::string& str = "") {}
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << "\t";
    }
private:
};

/*
 * --------------- LogFormatter ---------------
*/

LogFormatter::LogFormatter(const std::string& pattern)
: m_pattern(pattern){
    parse();
}

void LogFormatter::parse() {
    // Parse the format pattern
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string text;
    for (size_t i = 0; i < m_pattern.size(); ++i){
        // text information
        if (m_pattern[i] != '%') {
            text.append(1, m_pattern[i]);
            continue;
        }

        if ((i + 1) < m_pattern.size()){
            // For '%'
            if (m_pattern[i + 1] == '%') {
                text.append(1, '%');
                continue;
            }
        }

        // format pattern parsing (starting by '%')

        size_t n = i + 1; // the next char in m_pattern
        int fmt_status = 0;
        std::string fmt_str;
        std::string str;

        for (size_t fmt_begin = 0; n < m_pattern.size(); ++n){
            if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}')) {  // isspace(m_pattern[n])
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if (fmt_status == 0){
                if (m_pattern[n] == '{') {
                    fmt_status = 1;
                    str = m_pattern.substr(i + 1, n - i - 1); // before '{'
                    fmt_begin = n;
                    continue;
                }
                if (n == m_pattern.size() - 1 && str.empty()) {
                    str = m_pattern.substr(n, 1);
                    continue;
                }
            }
            if (fmt_status == 1) {
                if (m_pattern[n] == '}') {
                    fmt_status = 0;
                    fmt_str = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    ++n;
                    break;
                }
            }
        }

        if (fmt_status == 0) {
            // No '{' in format pattern
            if (!text.empty()){
                vec.push_back(std::make_tuple(text, std::string(), 0));
                text.clear();
            }
            vec.push_back(std::make_tuple(str, fmt_str, 1));
            i = n - 1; // flash the index
        } 
        else if (fmt_status == 1) {
            // No paired brace in format pattern 
            std::cout << "Pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt_str, 0));
        } 
        //std::cout << "status: " << fmt_status << ", i: " << i << ", str length: " << str.size() << std::endl;
    }
    if (!text.empty()){
        vec.push_back(std::make_tuple(text, std::string(), 0));
        text.clear();
    }

    /*   
    * %m -- message 
    * %p -- level
    * %r -- elapse from starting
    * %t -- threadID
    * %n -- newline
    * %d -- time
    * %f -- filename
    * %l -- linenumber
    * %T -- tab
    */
    static std::map<std::string, std::function<FormatItem::ptr(const std::string)> > s_format_items = {
        {"m", [](const std::string& fmt){ return FormatItem::ptr(new MessageFormatItem(fmt)); }},
        {"p", [](const std::string& fmt){ return FormatItem::ptr(new LevelFormatItem(fmt)); }},
        {"r", [](const std::string& fmt){ return FormatItem::ptr(new ElapseFormatItem(fmt)); }},
        {"t", [](const std::string& fmt){ return FormatItem::ptr(new ThreadIDFormatItem(fmt)); }},
        {"n", [](const std::string& fmt){ return FormatItem::ptr(new NewLineFormatItem(fmt)); }},
        {"d", [](const std::string& fmt){ return FormatItem::ptr(new DateTimeFormatItem(fmt)); }},
        {"f", [](const std::string& fmt){ return FormatItem::ptr(new FileNameFormatItem(fmt)); }},
        {"l", [](const std::string& fmt){ return FormatItem::ptr(new LineNumberFormatItem(fmt)); }},
        {"T", [](const std::string& fmt){ return FormatItem::ptr(new TabFormatItem(fmt)); }}
    };
    
    for (auto& i : vec) {
        if (std::get<2>(i) == 0) {
            // text information
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        }
        else {
            auto it = s_format_items.find(std::get<0>(i));
            if (it == s_format_items.end()) {
                // No found
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
            }
            else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
        //std::cout << std::get<0>(i) << " - " << std::get<1>(i) << " - " << std::get<2>(i) << std::endl;
    }
}

std::string LogFormatter::format (LogLevel::Level level, LogEvent::ptr event){
    std::stringstream ss; 
    for (auto& i: m_items){
        i->format(ss, level, event);
    }
    return ss.str();
}

/*
 * --------------- LogAppender ---------------
*/
void StdoutLogAppender::log (LogLevel::Level level, LogEvent::ptr event){
    if (level >= m_level) {
        // print to consoler
        std::cout << m_formatter->format(level, event);
    }
}

FileLogAppender::FileLogAppender (const std::string& filename)
: m_filename(filename) {
    m_filestream.open(m_filename);
    if (!m_filestream.is_open()){

        // TODO: exception dealing
        std::cout << "File opening failed. " << std::endl;
        exit(1);
    }
}
void FileLogAppender::log (LogLevel::Level level, LogEvent::ptr event) {
    if (level >= m_level) {
        // print to file
        m_filestream << m_formatter->format(level, event);
    }
}
bool FileLogAppender::reopen (){
    if (m_filestream){
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return m_filestream.is_open();
}

/*
 * --------------- Logger ---------------
*/
Logger::Logger(const std::string& LogName)
: m_logname(LogName), m_level(LogLevel::ALL){
    m_formatter.reset(new LogFormatter());
}

void Logger::addAppender (LogAppender::ptr appender){
    if (!appender->getFormatter()){
        // No formatter
        appender->setFormatter(m_formatter);
    }
    m_appenders.push_back(appender);
}
void Logger::delAppender (LogAppender::ptr appender){
    for (auto it = m_appenders.begin();
              it != m_appenders.end(); ++it){
            if (*it == appender) {
                m_appenders.erase(it);
                break;
            }
        }
}
void Logger::log(LogLevel::Level level, LogEvent::ptr event){
    if (level >= m_level){
        for (auto& i : m_appenders){
            i->log(level, event);
        }
    }
}

/*
 * --------------- LoggerManager ---------------
*/
LoggerManager::LoggerManager (){
    m_root.reset(new Logger);
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
}
std::shared_ptr<Logger> LoggerManager::getLogger(const std::string& name) {
    auto it = m_loggers.find(name);
    if (it != m_loggers.end()) {
        return it->second;
    }
    return m_root;
}

void LoggerManager::init() {

}


} // end of namespace