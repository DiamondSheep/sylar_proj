#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include <memory>
#include <boost/lexical_cast.hpp>
#include "log.hpp"

namespace sylar {

class ConfigVarBase {
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase (const std::string& name , const std::string& description)
    : m_name(name), m_description(description) {}
    virtual ~ConfigVarBase() {}

    const std::string& getName () { return m_name; }
    const std::string & getDescription() { return m_description; }

    virtual std::string toString() = 0;
    virtual bool fromString (const std::string& val) = 0;
protected:
    std::string m_name;
    std::string m_description;
};

// Derivated classes
template <class T>
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
            return boost::lexical_cast<std::string> (m_val);
        } catch (std::exception& e) {
            SYLAR_LOG_LEVEL(SYLAR_LOG_ROOT(), LogLevel::ALL) << "ConfigVar::toString exception" << e.what() << " convert " << typeid(m_val).name() << " to string.";
        }
        return "";
    }
    bool fromString (const std::string& val) override {
        try {
            m_val = boost::lexical_cast<T> (val);
            return true;
        } catch (std::exception& e) {
            SYLAR_LOG_LEVEL(SYLAR_LOG_ROOT(), LogLevel::ALL) << "ConfigVar::fromString exception" << e.what() << " convert string to " << typeid(m_val).name() << ".";
        }
        return false;
    }
private:
    T m_val;
};

class Config {
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    // Create a ConfigVar if m_data is empty
    template <class T>
    // Only "typename" make the name to be a class
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
            const T& default_value, const std::string& description = "") {
        auto tmp = Lookup<T> (name);
        if (tmp) {
            // found
            SYLAR_LOG_LEVEL(SYLAR_LOG_ROOT(), LogLevel::ALL) << "Lookup name=" << name << " exists";
        }
        if (name.find_first_not_of ("abcdefghijklmnopqrstuvwxyzABCDEFGHIMNOPQRSTUVWXYZ")!=std::string::npos) {
            SYLAR_LOG_LEVEL(SYLAR_LOG_ROOT(), LogLevel::ALL) << "Lookup name invalid" << name;
            throw std::invalid_argument(name);
        }
        // Create new ConfigVar
        typename ConfigVar<T>::ptr v(new ConfigVar<T> (name, default_value, description));
        s_data[name] = v;
    }
    template <class T>
    static typename ConfigVar<T>::ptr Lookup (const std::string& name) {
        auto it = s_data.find(name);
        if (it == s_data.end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> > (it->second);
    }
            
private:
    static ConfigVarMap s_data;
};

}

#endif