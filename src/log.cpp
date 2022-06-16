#include "log.hpp"

namespace sylar{
/*
 * --------------- LogFormatter ---------------
*/

/*
 * --------------- LogAppender ---------------
*/
void LogAppender::log (LogLevel::Level level, LogEvent::ptr event){

}

/*
 * --------------- Logger ---------------
*/
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

// end of namespace
}