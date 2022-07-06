#include "config.hpp"
#include "log.hpp"
#include <yaml-cpp/yaml.h>

/*
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
sylar::ConfigVar<std::map<std::string, int>>::ptr g_int_map_config 
    = sylar::Config::Lookup ("system.int_map", std::map<std::string, int> {{"val", 2}}, "system int map");
sylar::ConfigVar<std::unordered_map<std::string, int>>::ptr g_int_umap_config 
    = sylar::Config::Lookup ("system.int_umap", std::unordered_map<std::string, int> {{"val", 2}}, "system int umap");

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
    YAML::Node root = YAML::LoadFile("../conf/test.yml");
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
    auto m = g_int_map_config->getValue();
    for (auto& i : m) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "item: " << i.first << " " << i.second;
    }
    auto um = g_int_umap_config->getValue();
    for (auto& i : um) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "item: " << i.first << " " << i.second;
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
    m = g_int_map_config->getValue();
    for (auto& i : m) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "item: " << i.first << " " << i.second;
    }
    um = g_int_umap_config->getValue();
    for (auto& i : um) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "item: " << i.first << " " << i.second;
    }
}

class Person {
public:
    std::string m_name = "";
    int m_age = 0;
    bool m_sex = 0;
    std::string toString () const {
        std::stringstream ss;
        ss << "[ Person information: name=" << m_name
           << " age=" << m_age
           << " sex=" << m_sex
           << " ]";
        return ss.str();
    }
    bool operator== (const Person& p) const {
        if (p.m_name == m_name && 
            p.m_age == m_age && 
            p.m_sex == m_sex) {
            return true;
        }
        else { return false; }
    }
};

// a example to convert complex type
namespace sylar {

// Full Template Specialization
template<>
class LexicalCast<std::string, Person> {
public:
    Person operator() (const std::string& string) {
        YAML::Node node = YAML::Load(string);
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};

template<>
class LexicalCast<Person, std::string> {
public:
    std::string operator() (const Person& p) {
        YAML::Node node;
        node["name"] = p.m_name;
        node["age"] = p.m_age;
        node["sex"] = p.m_sex;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};
}

sylar::ConfigVar<Person>::ptr g_person_config 
    = sylar::Config::Lookup ("class.person", Person(), "class person");
sylar::ConfigVar<std::map<std::string, Person>>::ptr g_map_person_config
    = sylar::Config::Lookup("class.map", std::map<std::string, Person>(), "class map");
sylar::ConfigVar<std::map<std::string, std::vector<Person>>>::ptr g_mapvec_config
    = sylar::Config::Lookup("class.mapvec", std::map<std::string, std::vector<Person>>(), "class mapvec");

void test_class () {
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_person_config->getValue().toString() << " - " << g_person_config->toString();
    auto m = g_map_person_config->getValue();
    for (auto& i: m) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << i.first << " - " << i.second.toString();
    }
    auto v = g_mapvec_config->getValue();
    for (auto& i: v) {
        for (auto& j: i.second) {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << j.toString();
        }
    }

    std::cout << " --- " << std::endl;
    YAML::Node node = YAML::LoadFile("../conf/log.yml");
    sylar::Config::LoadFromYaml(node);

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_person_config->getValue().toString() << " - " << g_person_config->toString();
    m = g_map_person_config->getValue();
    for (auto& i: m) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << i.first << " - " << i.second.toString();
    }
    v = g_mapvec_config->getValue();
    for (auto& i: v) {
        for (auto& j: i.second) {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << j.toString();
        }
    }
}

void test_callback() {
    g_person_config->addListener(0, [](const Person& old_value, const Person& new_value) {
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "old value: " << old_value.toString() << " new value: " << new_value.toString();
    });
    YAML::Node node = YAML::LoadFile("../conf/log.yml");
    sylar::Config::LoadFromYaml(node);
}
*/
void test_log() {
    static sylar::Logger::ptr system_log = SYLAR_LOG_NAME("system");
    // Different from the original version
    // You have to add the appender by your self
    system_log->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));
    SYLAR_LOG_INFO(system_log) << "hello system" << std::endl;
    std::cout << sylar::SltLoggerMgr::GetInstance()->toYamlString() << std::endl; 
    YAML::Node root = YAML::LoadFile("../conf/test.yml");
    sylar::Config::LoadFromYaml(root);
    std::cout << sylar::SltLoggerMgr::GetInstance()->toYamlString() << std::endl; 
    SYLAR_LOG_INFO(system_log) << "hello system" << std::endl;
}

int main(int argc, char* argv[]){
	SYLAR_LOG_ALL(SYLAR_LOG_ROOT()) << "config test\n";
    // test_yaml();
    // test_config();
    // test_class();
    // test_callback();
    test_log();
    SYLAR_LOG_ALL(SYLAR_LOG_ROOT()) << "config finished";
    return 0;
}