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
 * --------------- FormatItem ---------------
 */
class MessageFormatItem : public LogFormatter::FormatItem{
public:
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem{
public:
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << LogLevel::ToString(level);
    }
};

class ElapseFormatItem : public LogFormatter::FormatItem{
public:
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getElapse();
    }
};

// Note: Different from Sylar, we don't implement the getName() function

class ThreadIDFormatItem : public LogFormatter::FormatItem{
public:
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getThreadID();
    }
};

class FiberIDFormatItem : public LogFormatter::FormatItem{
public:
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getFiberID();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem{
public:
    DateTimeFormatItem (const std::string& format = "")
    : m_format(format) {
    }
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getTime();
    }
private:
    std::string m_format;
};

class FileNameFormatItem : public LogFormatter::FormatItem{
public:
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getFileName();
    }
};

class LineNumberFormatItem : public LogFormatter::FormatItem{
public:
    virtual void format(std::ostream& os, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getLineNumber(); // TODO
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem{
public:
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


/*
 * --------------- LogFormatter ---------------
*/

LogFormatter::LogFormatter(const std::string& pattern)
: m_pattern(pattern){

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
            if (m_pattern[i + 1] == '%') {
                text.append(1, '%');
                continue;
            }
        }

        // format pattern parsing (starting by '%')
        size_t n = i + 1; // the next char in m_pattern
        size_t fmt_begin = 0;
        int fmt_status = 0;
        std::string fmt_str;
        std::string str;

        while (n < m_pattern.size() && m_pattern[n] != '%'){
            if (isspace(m_pattern[n])) {
                break;
            }
            if (fmt_status == 0){
                if (m_pattern[n] == '{') {
                    fmt_status = 1;
                    str = m_pattern.substr(i + 1, n - i - 1); // before '{'
                    fmt_begin = n;
                    continue;
                }
            }
            if (fmt_status == 1) {
                if (m_pattern[n] == '}') {
                    fmt_status = 2;
                    fmt_str = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    break;
                }
            }
            ++n;
        } // end of parsing
        
        if (!text.empty()){
            vec.push_back(std::make_tuple(text, "", 0));
            text.clear();
        }

        if (fmt_status == 0) {
            // No '{' in format pattern
            str = m_pattern.substr(i + 1, n - i - 1);
            vec.push_back(std::make_tuple(str, fmt_str, 1));
        } 
        else if (fmt_status == 1) {
            // No paired brace in format pattern 
            std::cout << "Pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt_str, 0));
        } 
        else if (fmt_status == 2) {
            // Paired brace in format pattern
            vec.push_back(std::make_tuple(str, fmt_str, 1));
        }
        i = n; // flash the index
    }

    if (!text.empty()){
        vec.push_back(std::make_tuple(text, "", 0));
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
    */
    static std::map<std::string, std::function<FormatItem::ptr(const std::string)> > s_format_items = {
        {"m", [](const std::string& fmt){ return FormatItem::ptr(new MessageFormatItem()); }},
        {"p", [](const std::string& fmt){ return FormatItem::ptr(new LevelFormatItem()); }},
        {"r", [](const std::string& fmt){ return FormatItem::ptr(new ElapseFormatItem()); }},
        {"t", [](const std::string& fmt){ return FormatItem::ptr(new ThreadIDFormatItem()); }},
        {"n", [](const std::string& fmt){ return FormatItem::ptr(new NewLineFormatItem()); }},
        {"d", [](const std::string& fmt){ return FormatItem::ptr(new DateTimeFormatItem(fmt)); }},
        {"f", [](const std::string& fmt){ return FormatItem::ptr(new FileNameFormatItem()); }},
        {"l", [](const std::string& fmt){ return FormatItem::ptr(new LineNumberFormatItem()); }},
        //{"s", [](const std::string& fmt){ return FormatItem::ptr(new StringFormatItem(fmt)); }}
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
        std::cout << std::get<0>(i) << " - " << std::get<1>(i) << " - " << std::get<2>(i) << std::endl;
    }
}

std::string LogFormatter::format (LogLevel::Level level, LogEvent::ptr event){
    std::stringstream ss;
    for (auto& i: m_items){
        i->format(ss, level, event);
    }
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
: m_logname(LogName), m_level(LogLevel::DEBUG){

}

void Logger::addAppender (LogAppender::ptr appender){
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


} // end of namespace