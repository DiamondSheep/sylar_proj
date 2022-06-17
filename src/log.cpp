#include "log.hpp"

namespace sylar{
/*
 * --------------- LogLevel ---------------
*/
static const char* LogLevel::ToString(LogLevel::Level level) {
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
}

/* 
 * --------------- FormatItem ---------------

 * %m -- message
 * %p -- level
 * %r -- elapse from starting
 * %t -- threadID
 * %n -- newline
 * %d -- time
 * %f -- filename
 * %l -- linenumber
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


std::string LogFormatter::format(LogLevel::Level level, LogEvent::ptr event){
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
        std::cout << m_formatter->format(event);
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
        m_filestream << m_formatter->format(event);
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