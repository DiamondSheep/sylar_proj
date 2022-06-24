#include "config.hpp"
#include "log.hpp"

int main(int argc, char* argv[]){
    auto l = sylar::SltLoggerMgr::GetInstance()->getRoot();
	SYLAR_LOG_LEVEL(l, sylar::LogLevel::ALL) << "test";
    return 0;
}