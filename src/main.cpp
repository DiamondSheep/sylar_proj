
#include "log.hpp" 
#include <iostream>
 
using namespace sylar;

int main(int argc, char* argv[]){ 
	std::cout << "-- Project: sylar" << std::endl; 
	Logger logger("demo");
	std::string pattern;
	if (argc > 1)
		pattern = argv[1];
	else
		pattern = "[ %l ] %m %n";
	LogFormatter::ptr formatter (new LogFormatter(pattern));
	LogAppender::ptr stdapp (new StdoutLogAppender);
	stdapp->setFormatter(formatter);
	logger.addAppender(stdapp);
	LogEvent::ptr event( new LogEvent(__FILE__, __LINE__, 0, 1, 2, time(0)) );

	event->getSS() << "Hello";
	
	logger.log(LogLevel::ALL, event);
	
	return 0; 
}
