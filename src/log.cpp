#include "log.hpp"
#include "config.hpp"

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

LogLevel::Level LogLevel::FromString(const std::string& str){
    if (str == "ALL") {
        return LogLevel::ALL;
    }
    if (str == "DEBUG") {
        return LogLevel::DEBUG;
    }
    if (str == "INFO") {
        return LogLevel::INFO;
    }
    if (str == "WARN") {
        return LogLevel::WARN;
    }
    if (str == "FATAL") {
        return LogLevel::FATAL;
    }
    return LogLevel::OFF;
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
                        logger->setLevel(level);
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
    if (pattern.empty()){
        // If no given pattern, 
        // it will be initialized with following pattern 
        m_pattern = "%d{%Y-%m-%d %H:%M:%S}%T[%p]%T<%f:%l>%T%t%T%m %n";
    }
    // Parsing the pattern
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
            m_error = true;
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
                m_error = true;
            }
            else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
        //std::cout << std::get<0>(i) << " - " << std::get<1>(i) << " - " << std::get<2>(i) << std::endl;
    }
}

std::string LogFormatter::format (std::shared_ptr<Logger> logger_ptr, LogEvent::ptr event){
    std::stringstream ss; 
    LogLevel::Level level = logger_ptr->getLevel();
    for (auto& i: m_items){
        i->format(ss, level, event);
    }
    return ss.str();
}

/*
 * --------------- LogAppender ---------------
 *  Only if the given level higher than the internal level
 *  E.g.: appender(LogLevel::WARN) 
 *        appender.log(LogLevel::DEBUG, event); -> information 
 *        appender.log(LogLevel::ERROR, event); -> Nothing
 *  However, only Logger can control the level, so we set the appender level to the lowest. 
 */
void StdoutLogAppender::log (std::shared_ptr<Logger> logger_ptr,LogEvent::ptr event){
    // print to consoler
    if (m_formatter){
        std::cout << m_formatter->format(logger_ptr, event);
    }else {
        std::cout << "No formatter" << std::endl;
    }
}

std::string StdoutLogAppender::toYamlString() const {
    YAML::Node node;
    node["type"] = "StdoutLogAppender";
    if (m_formatter){
        node["pattern"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

void StdoutLogAppender::setFormatter (const std::string& pattern) {
    LogFormatter::ptr new_fmt (new LogFormatter(pattern));
    if (new_fmt->isError()) { // check
        std::cout << "StdoutLogAppender value=" 
                  << pattern << " invalid pattern. " 
                  << std::endl;
        return;
    }
    setFormatter(new_fmt);
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

void FileLogAppender::log (std::shared_ptr<Logger> logger_ptr, LogEvent::ptr event) {
    m_filestream << m_formatter->format(logger_ptr, event);
}

bool FileLogAppender::reopen (){
    if (m_filestream){
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return m_filestream.is_open();
}

void FileLogAppender::setFormatter (const std::string& pattern) {
    LogFormatter::ptr new_fmt (new LogFormatter(pattern));
    if (new_fmt->isError()) { // check
        std::cout << "FileLogAppender value=" 
                  << pattern << " invalid pattern. " 
                  << std::endl;
        return;
    }
    setFormatter(new_fmt);
}

std::string FileLogAppender::toYamlString() const {
    YAML::Node node;
    node["type"] = "FileLogAppender";
    node["file"] = m_filename;
    if (m_formatter){
        node["pattern"] = m_formatter->getPattern();
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
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
void Logger::clearAppender() {
    m_appenders.clear();    
}
void Logger::log(LogLevel::Level level, LogEvent::ptr event){
    if (level >= m_level){
        auto p = shared_from_this();
        if (! m_appenders.empty()) { 
            for (auto& i : m_appenders){
                i->log(p, event);
            }
        }
        else {
            std::cout << "Appender empty." << std::endl;
        }
    }
}

LogFormatter::ptr Logger::getFormatter() const {
    return m_formatter;
}

void Logger::setFormatter (const LogFormatter::ptr formatter) {
    m_formatter = formatter;
}

void Logger::setFormatter (const std::string& pattern) {
    LogFormatter::ptr new_fmt (new LogFormatter(pattern));
    if (new_fmt->isError()) { // check
        std::cout << "Logger setFormatter name=" 
                  << m_logname << " value=" 
                  << pattern << " invalid pattern. " 
                  << std::endl;
        return;
    }
    setFormatter(new_fmt);
}

std::string Logger::toYamlString() const {
    YAML::Node node;
    node["name"] = m_logname;
    node["level"] = LogLevel::ToString(m_level);
    for (auto& i : m_appenders) {
        node["appender"].push_back(YAML::Load(i->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

/*
 * --------------- LoggerManager ---------------
*/
LoggerManager::LoggerManager (){
    m_root.reset(new Logger);
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
    init();
}

void LoggerManager::addLogger (const std::string& name, std::shared_ptr<Logger> logger){
    if (getLogger(name) != m_root) {
        std::cout << "Logger exists. " << std::endl;
    }
    m_loggers[name] = logger;
}

std::shared_ptr<Logger> LoggerManager::getLogger(const std::string& name) {
    auto it = m_loggers.find(name);
    if (it != m_loggers.end()) {
        return it->second;
    }
    // create new logger
    std::shared_ptr<Logger> logger (new Logger (name));
    m_loggers[name] = logger;
    return logger;
}

struct AppenderDefinition {
    int type = 0;
    std::string pattern;
    std::string file;
    bool operator== (const AppenderDefinition& def) const {
        return type == def.type &&
               pattern == def.pattern && 
               file == def.file;
    }
};

struct LogDefinition {
    std::string name;
    LogLevel::Level level = LogLevel::OFF;
    std::string pattern;
    std::vector<AppenderDefinition> appenders;
    bool operator== (const LogDefinition& logdef) const {
        return name == logdef.name &&
               level == logdef.level && 
               pattern == logdef.pattern && 
               appenders == logdef.appenders;
    }
    bool operator< (const LogDefinition& logdef) const {
         return name < logdef.name;
    }
};

/*
// Full Template Specialization
template<>
class LexicalCast<std::string, std::set<LogDefinition> > {
public:
    std::set<LogDefinition> operator() (const std::string& string) const {
        YAML::Node node = YAML::Load(string);
        typename std::set<LogDefinition> s;
        for (size_t i = 0; i < node.size(); ++i) {
            LogDefinition def;
            //YAML::Node node = node[i]; 
            if (!node["name"].IsDefined()) {
                std::cout << "Log config error: name is not defined. " << std::endl;
                return s;
            }
            def.name = node["name"].as<std::string>();
            def.level = LogLevel::FromString(node["level"].IsDefined() ? node["level"].as<std::string>() : "");
            if (node["appender"].IsDefined()){
                // Appender 
                for (size_t j = 0; j < node["appender"].size(); ++j) {
                    auto item = node["appender"][j];
                    // type 
                    if (!item["type"].IsDefined()) {
                        std:: cout << "Log config error: appender type is NULL, " << item << std::endl;
                        continue;
                    }
                    std::string type = item["type"].as<std::string>();
                    AppenderDefinition apDefine;
                    if (type == "FileLogAppender") {
                        apDefine.type = 1;
                        // file (only for FileLogAppender)
                        if (!item["file"].IsDefined()) {
                            std::cout << "Log config error: fileappender file is not defined. " << std::endl;
                            continue;
                        }
                        apDefine.file = item["file"].as<std::string>();
                    }
                    else if (type == "StdoutLogAppender") {
                        apDefine.type = 2;
                    }
                    else {
                        std::cout << "Log config error: appender type is invalid. " << std::endl;
                        continue;
                    }
                    // pattern
                    if (item["pattern"].IsDefined()) {
                        apDefine.pattern = item["pattern"].as<std::string>();
                    }
                    def.appenders.push_back(apDefine);
                }
            }   
            s.insert(def);
        }
        return s;   
    }
};
*/

template<>
class LexicalCast<std::string, LogDefinition> {
public:
    LogDefinition operator() (const std::string& string) const {
        YAML::Node node = YAML::Load(string);
        LogDefinition def;
        
        // warining if name is not found
        if (!node["name"].IsDefined()) {
            std::cout << "Log config error: name is not defined. " << std::endl;
            // return def;
            exit(0); 
        }
        
        def.name = node["name"].as<std::string>();
        def.level = LogLevel::FromString(node["level"].IsDefined() ? node["level"].as<std::string>() : "");
        if (node["appender"].IsDefined()){
            // Appender 
            for (size_t i = 0; i < node["appender"].size(); ++i) {
                auto item = node["appender"][i];
                // type 
                if (!item["type"].IsDefined()) {
                    std:: cout << "Log config error: appender type is NULL, " << item << std::endl;
                    continue;
                }
                std::string type = item["type"].as<std::string>();
                AppenderDefinition apDefine;
                if (type == "FileLogAppender") {
                    apDefine.type = 1;
                    // file (only for FileLogAppender)
                    if (!item["file"].IsDefined()) {
                        std::cout << "Log config error: fileappender file is not defined. " << std::endl;
                        continue;
                    }
                    apDefine.file = item["file"].as<std::string>();
                }
                else if (type == "StdoutLogAppender") {
                    apDefine.type = 2;
                }
                else {
                    std::cout << "Log config error: appender type is invalid. " << std::endl;
                    continue;
                }
                // pattern
                if (item["pattern"].IsDefined()) {
                    apDefine.pattern = item["pattern"].as<std::string>();
                }
                def.appenders.push_back(apDefine);
            }
        }
        
        return def;
    }
};


template<>
class LexicalCast<LogDefinition, std::string> {
public:
    std::string operator() (const LogDefinition& def) {
        YAML::Node node;
        node["name"] = def.name;
        node["level"] = LogLevel::ToString(def.level);
        for (auto& ap: def.appenders) {
            YAML::Node apNode;
            if (ap.type == 1){
                // FileLogAppender
                apNode["type"] = "FileLogAppender";
                apNode["file"] = ap.file;
            }else if (ap.type == 2) {
                // StdoutAppender
                apNode["type"] = "StdoutAppender";
            }else{
                std::cout << "Cast Error: invalid type in appender" << std::endl;
            }
            if (!ap.pattern.empty()){
                //TODO: Some bugs exist
                apNode["pattern"] = ap.pattern;
            }
            else {
                std::cout << "appender pattern is empty. " << std::endl;
            }
            node["appenders"].push_back(apNode);
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/*
template<>
class LexicalCast<std::set<LogDefinition>, std::string> {
public:
    std::string operator() (const std::set<LogDefinition>& s) {
        std::stringstream ss;
        for (auto& def: s) {
            YAML::Node node;
            node["name"] = def.name;
            node["level"] = LogLevel::ToString(def.level);
            
            for (auto& ap: def.appenders) {
                YAML::Node apNode;
                if (ap.type == 1){
                    // FileLogAppender
                    apNode["type"] = "FileLogAppender";
                    apNode["file"] = ap.file;
                }else if (ap.type == 2) {
                    // StdoutAppender
                    apNode["type"] = "StdoutAppender";
                }else{
                    std::cout << "Cast Error: invalid type in appender" << std::endl;
                }
                if (!ap.pattern.empty()){
                    std::cout << ap.pattern << std::endl;
                    apNode["pattern"] = ap.pattern;
                }
                node["appenders"].push_back(apNode);
            }
            ss << node;
        }
        return ss.str();
    }
};
*/

// Config
sylar::ConfigVar<std::set<LogDefinition> >::ptr g_log_defines = 
      sylar::Config::Lookup("logs", std::set<LogDefinition> (), "logs config");

struct LogIniter {
    LogIniter () {
        // add callback function to config
        g_log_defines->addListener(0xF1E231, [](const std::set<LogDefinition>& old_value, 
                                                const std::set<LogDefinition>& new_value) {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "on logger changed";
            // operations
            for (auto& i : new_value) {
                auto it = old_value.find(i);
                std::shared_ptr<Logger> logger;
                if (it == old_value.end()) {
                    // add new logger
                    logger = SYLAR_LOG_NAME(i.name);
                    }
                else {
                    if (!(i == *it)){
                        // modify
                        logger = SYLAR_LOG_NAME(i.name);
                    }
                    else{
                        continue;
                    }
                }
                // set level
                logger->setLevel(i.level);
                // set formatter (logger)
                if (!i.pattern.empty()) {
                    logger->setFormatter(i.pattern);
                }
                // clear appenders
                logger->clearAppender();
                // ---------- add ---------- 
                for (auto& a : i.appenders) {
                    LogAppender::ptr ap;
                    if (a.type == 1) {
                        // FileLogAppender
                        ap.reset(new FileLogAppender(a.file));
                        // set formatter (appender)
                        if (!a.pattern.empty()) {
                            ap->setFormatter(a.pattern);
                        }
                    } 
                    else if (a.type == 2) {
                        // StdoutLogAppender
                        ap.reset(new StdoutLogAppender);
                        // set formatter (appender)
                        if (!a.pattern.empty()) {
                            ap->setFormatter(a.pattern);
                        }
                    }
                    else {
                        std::cout << "LogIniter: error type=" << a.type << std::endl;
                    }
                    logger->addAppender(ap);
                }
            
            for (auto& i : old_value) {
                auto it = new_value.find(i);
                if (it == new_value.end()) {
                    // ---------- delete ---------- 
                    auto logger = SYLAR_LOG_NAME(i.name);
                    logger->setLevel(LogLevel::OFF);
                    logger->clearAppender();
                }
            }
        }
        });
    }
};

static LogIniter __log_init;


void LoggerManager::init() {

}

std::string LoggerManager::toYamlString() const {
    YAML::Node node;
    for (auto & i : m_loggers) {
        node.push_back(YAML::Load(i.second->toYamlString()));
    }
    std::stringstream ss;
    ss << node;
    return ss.str();
}

} // end of namespace