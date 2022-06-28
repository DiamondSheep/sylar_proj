#include "config.hpp"
#include "log.hpp"
#include <yaml-cpp/yaml.h>

sylar::ConfigVar<int>::ptr g_int_val_config 
    = sylar::Config::Lookup ("system.port", (int)8080, "system port");
sylar::ConfigVar<float>::ptr g_float_val_config 
    = sylar::Config::Lookup ("system.value", (float)10.2f, "system value");
sylar::ConfigVar<std::vector<int>>::ptr g_int_vec_config 
    = sylar::Config::Lookup ("system.int_vec", std::vector<int> {1,2}, "system int vector");
sylar::ConfigVar<std::list<int>>::ptr g_int_list_config 
    = sylar::Config::Lookup ("system.int_list", std::list<int> {0,3}, "system int list");
sylar::ConfigVar<std::set<int>>::ptr g_int_set_config 
    = sylar::Config::Lookup ("system.int_set", std::set<int> {25,12}, "system int set");
sylar::ConfigVar<std::unordered_set<int>>::ptr g_int_uset_config 
    = sylar::Config::Lookup ("system.int_uset", std::unordered_set<int> {14,2}, "system int unordered set");


void print_yaml (const YAML::Node& node, int level=0) {
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
    else {
        std::cout << "Unsupported type" << std::endl;
        exit(1);
    }
}

void test_yaml () {
    YAML::Node root = YAML::LoadFile("../conf/log.yml");
    print_yaml(root, 0);
}

void test_config () {
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "BEFORE: ";
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_int_val_config->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_float_val_config->toString();
    auto v = g_int_vec_config->getValue();
    for (auto& i : v) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "item: " << i;
    }
    auto l = g_int_list_config->getValue();
    for (auto& i : l) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "item: " << i;
    }
    auto s = g_int_set_config->getValue();
    for (auto& i : s) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "item: " << i;
    }
    auto us = g_int_uset_config->getValue();
    for (auto& i : us) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "item: " << i;
    }

    std::cout << " --- " << std::endl;
    YAML::Node node = YAML::LoadFile("../conf/log.yml");
    sylar::Config::LoadFromYaml(node);

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "AFTER:";
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_int_val_config->getValue();
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_float_val_config->toString();
    v = g_int_vec_config->getValue();
    for (auto& i : v) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "item: " << i;
    }
    l = g_int_list_config->getValue();
    for (auto& i : l) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "item: " << i;
    }
    s = g_int_set_config->getValue();
    for (auto& i : s) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "item: " << i;
    }
    us = g_int_uset_config->getValue();
    for (auto& i : us) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "item: " << i;
    }
}

int main(int argc, char* argv[]){
	SYLAR_LOG_ALL(SYLAR_LOG_ROOT()) << "-- config test\n";
    // test_yaml();
    test_config();

    return 0;
}