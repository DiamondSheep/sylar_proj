#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>
#include "log.hpp"

namespace sylar {

class ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase (const std::string& name , const std::string& description)
    : m_name(name), 
      m_description(description) {
        // transform to lower case
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
      }
    virtual ~ConfigVarBase() {}

    const std::string& getName () { return m_name; }
    const std::string & getDescription() { return m_description; }

    virtual std::string toString() = 0;
    virtual bool fromString (const std::string& val) = 0;
protected:
    std::string m_name;
    std::string m_description;
};

// From type F to type T
template<class F, class T>
class LexicalCast {
public:
    T operator () (const F& v) {
        return boost::lexical_cast<T>(v);
    }
};

/*
 * Partial Template Specialization
 */

// string to vector
template<class T>
class LexicalCast<std::string, std::vector<T> > {
public:
    std::vector<T> operator() (const std::string& string) {
        YAML::Node node = YAML::Load(string);
        typename std::vector<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str(""); // flesh stringstream 
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>() (ss.str()));
        }
        return vec;
    }
};

// vector to string
template<class F>
class LexicalCast<std::vector<F>, std::string> {
public:
    std::string operator() (const std::vector<F>& vec) {
        YAML::Node node;
        for (auto& i : vec) {
            node.push_back(YAML::Load(LexicalCast<F, std::string>() (i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// string to list
template<class T>
class LexicalCast<std::string, std::list<T> > {
public:
    std::list<T> operator() (const std::string& string) {
        YAML::Node node = YAML::Load(string);
        typename std::list<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str(""); // flesh stringstream 
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>() (ss.str()));
        }
        return vec;
    }
};

// list to string
template<class F>
class LexicalCast<std::list<F>, std::string> {
public:
    std::string operator() (const std::list<F>& vec) {
        YAML::Node node;
        for (auto& i : vec) {
            node.push_back(YAML::Load(LexicalCast<F, std::string>() (i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// string to set
template<class T>
class LexicalCast<std::string, std::set<T> > {
public:
    std::set<T> operator() (const std::string& string) {
        YAML::Node node = YAML::Load(string);
        typename std::set<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str(""); // flesh stringstream 
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>() (ss.str()));
        }
        return vec;
    }
};

// set to string
template<class F>
class LexicalCast<std::set<F>, std::string> {
public:
    std::string operator() (const std::set<F>& vec) {
        YAML::Node node;
        for (auto& i : vec) {
            node.push_back(YAML::Load(LexicalCast<F, std::string>() (i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// string to unordered_set
template<class T>
class LexicalCast<std::string, std::unordered_set<T> > {
public:
    std::unordered_set<T> operator() (const std::string& string) {
        YAML::Node node = YAML::Load(string);
        typename std::unordered_set<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str(""); // flesh stringstream 
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>() (ss.str()));
        }
        return vec;
    }
};

// unordered_set to string
template<class F>
class LexicalCast<std::unordered_set<F>, std::string> {
public:
    std::string operator() (const std::unordered_set<F>& vec) {
        YAML::Node node;
        for (auto& i : vec) {
            node.push_back(YAML::Load(LexicalCast<F, std::string>() (i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//TODO------------------------------------------------------------
// string to map
template<class T>
class LexicalCast<std::string, std::map<T> > {
public:
    std::map<T> operator() (const std::string& string) {
        YAML::Node node = YAML::Load(string);
        typename std::map<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str(""); // flesh stringstream 
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>() (ss.str()));
        }
        return vec;
    }
};

// map to string
template<class F>
class LexicalCast<std::map<F>, std::string> {
public:
    std::string operator() (const std::map<F>& vec) {
        YAML::Node node;
        for (auto& i : vec) {
            node.push_back(YAML::Load(LexicalCast<F, std::string>() (i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

/* 
 * Components: name, value, description
 * class FromStr  
    T operator() (const std::string&)
 * class ToStr
    std::string operator() (const T&)
 */
// Derivated classes
template <class T, 
          class FromStr=LexicalCast<std::string, T>, 
          class ToStr=LexicalCast<T, std::string> >
class ConfigVar : public ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVar> ptr;
    ConfigVar(const std::string& name, 
              const T& default_value, 
              const std::string& description)
    : ConfigVarBase(name, description),
      m_val(default_value) {

      }
    std::string toString() override {
        try {
            //return boost::lexical_cast<std::string> (m_val); // Directly convert to string
            return ToStr() (m_val);
        } catch (std::exception& e) {
            SYLAR_LOG_LEVEL(SYLAR_LOG_ROOT(), LogLevel::ALL) << "ConfigVar::toString exception" << e.what() << " convert " << typeid(m_val).name() << " to string.";
        }
        return "";
    }
    bool fromString (const std::string& val) override {
        try {
            //m_val = boost::lexical_cast<T> (val); // Directly convert from string
            setValue(FromStr() (val));
            return true;
        } catch (std::exception& e) {
            SYLAR_LOG_LEVEL(SYLAR_LOG_ROOT(), LogLevel::ALL) << "ConfigVar::fromString exception" << e.what() << " convert string to " << typeid(m_val).name() << ".";
        }
        return false;
    }
    const T getValue () const { return m_val; }
    void setValue (const T& val) { m_val = val; }
private:
    T m_val;
};

class Config {
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    
    // Create a ConfigVar if m_data is empty
    template <class T> // Only "typename" make the name to be a class
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
            const T& default_value, const std::string& description = "") {
        auto tmp = Lookup<T> (name);
        if (tmp) {
            // found
            SYLAR_LOG_LEVEL(SYLAR_LOG_ROOT(), LogLevel::ALL) << "Lookup name=" << name << " exists";
        }
        if (name.find_first_not_of ("abcdefghijklmnopqrstuvwxyzABCDEFGHIMNOPQRSTUVWXYZ._0123456789")!=std::string::npos) {
            SYLAR_LOG_LEVEL(SYLAR_LOG_ROOT(), LogLevel::ALL) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }
        // Create new ConfigVar
        typename ConfigVar<T>::ptr v(new ConfigVar<T> (name, default_value, description));
        s_data[name] = v;
        return v;
    }

    template <class T>
    static typename ConfigVar<T>::ptr Lookup (const std::string& name) {
        auto it = s_data.find(name);
        if (it == s_data.end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> > (it->second);
    }

    static void LoadFromYaml(const YAML::Node& root);

    static ConfigVarBase::ptr LookupBase(const std::string& name);
private:
    static ConfigVarMap s_data;
};

}

#endif