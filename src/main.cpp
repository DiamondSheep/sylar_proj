
#include "log.hpp" 
#include <iostream>
 
using namespace sylar;

int main(){ 
	std::cout << "-- Project: sylar" << std::endl; 

	std::string logName = "demo";
	Logger logger(logName);
	LogEvent event;
	StdoutLogAppender appender;
	
	logger.addAppender(std::make_shared<StdoutLogAppender>(appender));
	logger.log(LogLevel::ALL, std::make_shared<LogEvent>(event));

	LogFormatter formatter("text1 %m %i{ABC}text2%sylar{xxx}text3");
	formatter.parse();
	
	return 0; 
}
