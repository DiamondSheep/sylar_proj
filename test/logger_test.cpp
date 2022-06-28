#include <iostream>
#include "log.hpp" 
#include "utils.hpp"
#include "config.hpp"

using namespace sylar;

int main(int argc, char* argv[]){ 
	std::cout << "-- Project: sylar" << std::endl; 
	
	std::string pattern;
	if (argc > 1)
		pattern = argv[1];
	else
		pattern = "%d{%Y-%m-%d %H:%M:%S}%T[%p]%T<%f:%l>%T%t%T%m %n";

	// customized pattern	
	LogFormatter::ptr formatter (new LogFormatter(pattern));
	LogAppender::ptr stdapp (new StdoutLogAppender);
	LogAppender::ptr fileapp (new FileLogAppender("../data/log.txt"));
	fileapp->setFormatter(formatter);
	stdapp->setFormatter(formatter);
	std::shared_ptr<Logger> logger (new Logger("demo"));
	logger->addAppender(stdapp);
	logger->addAppender(fileapp);
	LogEvent::ptr event( new LogEvent(logger, LogLevel::ALL, __FILE__, __LINE__, 0, GetThreadID(), GetFiberID(), time(0)) );
	event->getSS() << "test stringstream";
	logger->log(LogLevel::ALL, event);
	
	SYLAR_LOG_LEVEL(logger, LogLevel::ALL) << "test level";
	SYLAR_LOG_FMT_LEVEL(logger, LogLevel::ALL, "test fmt", "aa");

	auto l = SltLoggerMgr::GetInstance()->getLogger("test");
	SYLAR_LOG_LEVEL(l, LogLevel::ALL) << "test mgr";

	return 0; 
}
