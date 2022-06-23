
#include "log.hpp" 
#include <iostream>
#include "utils.hpp"

using namespace sylar;

int main(int argc, char* argv[]){ 
	std::cout << "-- Project: sylar" << std::endl; 
	std::shared_ptr<Logger> logger (new Logger("demo"));
	std::string pattern;
	if (argc > 1)
		pattern = argv[1];
	else
		pattern = "%d{%Y-%m-%d %H:%M:%S}%T[%p] <%f:%l>%T%t%T%m %n";
	LogFormatter::ptr formatter (new LogFormatter(pattern));
	LogAppender::ptr stdapp (new StdoutLogAppender);
	LogAppender::ptr fileapp (new FileLogAppender("./log.txt"));
	fileapp->setFormatter(formatter);
	stdapp->setFormatter(formatter);
	logger->addAppender(stdapp);
	logger->addAppender(fileapp);
	LogEvent::ptr event( new LogEvent(logger, LogLevel::ALL, __FILE__, __LINE__, 0, GetThreadID(), GetFiberID(), time(0)) );
	event->getSS() << "test stringstream";
	
	logger->log(LogLevel::ALL, event);
	
	SYLAR_LOG_LEVEL(logger, LogLevel::ALL) << "test level";
	SYLAR_LOG_FMT_LEVEL(logger, LogLevel::ALL, "test fmt", "aa");

	auto l = SltLoggerMgr::GetInstance()->getLogger("test");
	l->addAppender(stdapp);
	SYLAR_LOG_LEVEL(l, LogLevel::ALL) << "test mgr";
	
	return 0; 
}
