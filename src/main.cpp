
#include "log.hpp" 
#include <iostream>
 
using namespace sylar;

int main(int argc, char* argv[]){ 
	std::cout << "-- Project: sylar" << std::endl; 

	std::string givenFormat = argv[1];

	std::string logName = "demo";
	Logger logger(logName);
	LogEvent event;
	StdoutLogAppender appender;
	
	logger.addAppender(std::make_shared<StdoutLogAppender>(appender));
	logger.log(LogLevel::ALL, std::make_shared<LogEvent>(event));

	LogFormatter formatter(givenFormat);
	formatter.parse();
	
	return 0; 
}
