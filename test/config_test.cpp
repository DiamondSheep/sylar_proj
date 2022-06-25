#include "config.hpp"
#include "log.hpp"
#include <yaml-cpp/yaml.h>

sylar::ConfigVar<int>::ptr g_int_val_config 
    = sylar::Config::Lookup ("system.port", (int)8080, "system port");
sylar::ConfigVar<float>::ptr g_float_val_config 
    = sylar::Config::Lookup ("system.value", (float)1.2f, "system value");

void print_yaml (const YAML::Node& node, int level) {
    if (node.IsScalar()) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ') << node.Scalar() << " - " << node.Tag() << " - " << level;
    }
    else if (node.IsNull()) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ') << "NULL - " << node.Tag() << level;
    }
    else if (node.IsMap()) {
        for (auto it = node.begin();
             it != node.end(); ++it) {
                SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ') << it->first << " - " << it->second.Tag() << " - " << level;
                print_yaml(it->second, level + 1);
             }
    }else if (node.IsSequence()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level * 4, ' ') << *it << " - " << (*it).Tag() << " - " << level;
            print_yaml((*it), level + 1);
        }
        
    }
}

void test_yaml () {
    YAML::Node root = YAML::LoadFile("../conf/log.yml");
    print_yaml(root, 0);
}

int main(int argc, char* argv[]){
	SYLAR_LOG_ALL(SYLAR_LOG_ROOT()) << "-- config test\n";

    SYLAR_LOG_ALL(SYLAR_LOG_ROOT()) << g_int_val_config->toString();
    SYLAR_LOG_ALL(SYLAR_LOG_ROOT()) << g_float_val_config->toString();
    test_yaml();

    return 0;
}