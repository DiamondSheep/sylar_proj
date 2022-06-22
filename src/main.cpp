
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
	//LogFormatter::ptr formatter (new LogFormatter(pattern));
	//LogAppender::ptr stdapp (new StdoutLogAppender);
	//stdapp->setFormatter(formatter);
	//logger->addAppender(stdapp);
	LogEvent::ptr event( new LogEvent(logger, LogLevel::ALL, __FILE__, __LINE__, 0, GetThreadID(), GetFiberID(), time(0)) );
	//event->getSS() << "Hello";
	
	//logger.log(LogLevel::ALL, event);
	std::cout << " Test macro " << std::endl;
	SYLAR_LOG_LEVEL(logger, LogLevel::ALL) << "test\n";
	
	return 0; 
}
