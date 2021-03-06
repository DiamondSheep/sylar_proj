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
#include <functional>

#include "log.hpp"
#include "threads.hpp"

namespace sylar {

class ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    typedef Mutex_RW MutexType; // consider more read cases
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
    virtual std::string getTypeName () const = 0;
protected:
    std::string m_name;
    std::string m_description;
};

// Convert From type F to type T
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

// string to map
template<class T>
class LexicalCast<std::string, std::map<std::string, T> > {
public:
    std::map<std::string, T> operator() (const std::string& string) {
        YAML::Node node = YAML::Load(string);
        typename std::map<std::string, T> map;
        std::stringstream ss;
        
        for (auto it = node.begin(); it != node.end(); ++it) {
            ss.str(""); // flesh stringstream 
            ss << it->second;
            map.insert(std::make_pair(it->first.Scalar(),
                        LexicalCast<std::string, T>() (ss.str())));
        }
        return map;
    }
};

// map to string
template<class F>
class LexicalCast<std::map<std::string, F>, std::string> {
public:
    std::string operator() (const std::map<std::string, F>& map) {
        YAML::Node node;
        for (auto& i : map) {
            node[i.first] = YAML::Load(LexicalCast<F, std::string>() (i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

// string to unordered_map
template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T> > {
public:
    std::unordered_map<std::string, T> operator() (const std::string& string) {
        YAML::Node node = YAML::Load(string);
        typename std::unordered_map<std::string, T> map;
        std::stringstream ss;
        
        for (auto it = node.begin(); it != node.end(); ++it) {
            ss.str(""); // flesh stringstream 
            ss << it->second;
            map.insert(std::make_pair(it->first.Scalar(),
                        LexicalCast<std::string, T>() (ss.str())));
        }
        return map;
    }
};

// map to string
template<class F>
class LexicalCast<std::unordered_map<std::string, F>, std::string> {
public:
    std::string operator() (const std::unordered_map<std::string, F>& map) {
        YAML::Node node;
        for (auto& i : map) {
            node[i.first] = YAML::Load(LexicalCast<F, std::string>() (i.second));
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
    typedef std::function<void (const T& old_value, const T& new_value)> on_change_callback;
    ConfigVar(const std::string& name, 
              const T& default_value, 
              const std::string& description)
    : ConfigVarBase(name, description),
      m_val(default_value) {

      }
    std::string toString() override {
        try {
            //return boost::lexical_cast<std::string> (m_val); // Directly convert to string
            MutexType::ReadLock lock(m_lock);
            return ToStr() (m_val);
        } catch (std::exception& e) {
            SYLAR_LOG_LEVEL(SYLAR_LOG_ROOT(), LogLevel::ALL) << "ConfigVar::toString exception" << e.what() << " convert " << typeid(m_val).name() << " to string.";
        }
        return "";
    }
    bool fromString (const std::string& val) override {
        try {
            //m_val = boost::lexical_cast<T> (val); // Directly convert from string
            // trigger the callback functions
            setValue(FromStr() (val)); 
            return true;
        } catch (std::exception& e) {
            SYLAR_LOG_LEVEL(SYLAR_LOG_ROOT(), LogLevel::ALL) << "ConfigVar::fromString exception" << e.what() << " convert string to " << typeid(m_val).name() << ".";
        }
        return false;
    }
    const T getValue () {
        MutexType::ReadLock lock(m_lock);
        return m_val; 
    }
    void setValue (const T& val) {
        { // to release the mutex
            if (val == m_val) {
                return;
            }
            MutexType::WriteLock lock(m_lock);
            // call the callback functions
            for (auto& f : m_callbacks) {
                f.second(m_val, val);
            }
        }
        MutexType::ReadLock lock(m_lock);
        m_val = val; 
    }
    std::string getTypeName () const override { return typeid(T).name(); }
    
    // listener operations
    // return the key of callback function
    uint64_t addListener(on_change_callback cb) {
        static uint64_t s_fun_id = 0;
        MutexType::WriteLock lock(m_lock);
        ++s_fun_id;
        m_callbacks[s_fun_id] = cb;
        return s_fun_id;
    }
    void delListener(uint64_t key) {
        MutexType::WriteLock lock(m_lock);
        m_callbacks.erase(key);
    }
    on_change_callback getListener (uint64_t key) {
        MutexType::ReadLock lock(m_lock);
        auto it = m_callbacks.find(key);
        return it == m_callbacks.end() ? nullptr : it->second;
    }
    void clearListener () {
        MutexType::WriteLock lock(m_lock);
        m_callbacks.clear();
    }

private:
    T m_val;
    // function group <key(int64_t, unique, hash), function>
    std::map<uint64_t, on_change_callback> m_callbacks;
    MutexType m_lock;
};

class Config {
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    typedef Mutex_RW MutexType;
    // Create a ConfigVar if m_data is empty
    template <class T> // Only "typename" make the name to be a class
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
            const T& default_value, const std::string& description = "") {
        MutexType::WriteLock lock(GetMutex());
        auto it = GetData().find(name);
        if (it != GetData().end()) {
            // exist
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T> > (it->second);
            if (tmp) {
                SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Lookup name=" << name << " exists";
                return tmp;
            }
            else {
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "" << name << " exists but type " << it->second->getTypeName() << " is not supported.";
                return nullptr;
            }
        } 
        // do not exist
        if (name.find_first_not_of ("abcdefghijklmnopqrstuvwxyzABCDEFGHIMNOPQRSTUVWXYZ._0123456789")!=std::string::npos) {
            SYLAR_LOG_LEVEL(SYLAR_LOG_ROOT(), LogLevel::ALL) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }
        // Create new ConfigVar
        typename ConfigVar<T>::ptr v(new ConfigVar<T> (name, default_value, description));
        GetData()[name] = v;
        return v;
    }

    template <class T>
    static typename ConfigVar<T>::ptr find (const std::string& name) {
        // return corresponding pointer by name
        MutexType::ReadLock lock(GetMutex());
        auto it = GetData().find(name);
        if (it == GetData().end()) {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << name << " does not exist.";
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> > (it->second);
    }

    static void LoadFromYaml(const YAML::Node& root);
    static ConfigVarBase::ptr LookupBase(const std::string& name);
    static void Visit(std::function<void(ConfigVarBase::ptr)> callback);
private:
    // for initialization
    static ConfigVarMap& GetData() {
        static ConfigVarMap s_data;
        return s_data;
    }
    static MutexType& GetMutex() {
        static MutexType s_lock;
        return s_lock;
    }
};

}

#endif