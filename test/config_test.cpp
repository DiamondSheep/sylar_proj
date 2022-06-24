#include "config.hpp"
#include "log.hpp"

sylar::ConfigVar<int>::ptr g_int_val_config 
    = sylar::Config::Lookup ("system.port", (int)8080, "system port");
sylar::ConfigVar<float>::ptr g_float_val_config 
    = sylar::Config::Lookup ("system.value", (float)1.2f, "system value");

int main(int argc, char* argv[]){
	SYLAR_LOG_ALL(SYLAR_LOG_ROOT()) << "-- config test\n";

    SYLAR_LOG_ALL(SYLAR_LOG_ROOT()) << g_int_val_config->toString();

    SYLAR_LOG_ALL(SYLAR_LOG_ROOT()) << g_float_val_config->toString();
    return 0;
}