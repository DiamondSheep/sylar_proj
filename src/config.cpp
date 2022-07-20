#include "config.hpp"

namespace sylar {

ConfigVarBase::ptr Config::LookupBase(const std::string& name){
    // find if name is in s_data
    // return ptr if it exists, or else nullptr
    MutexType::ReadLock lock(GetMutex());
    auto it = GetData().find(name);
    return it == GetData().end() ? nullptr : it->second;
}

/*
 * --------------- Config ---------------
 */
static void ListAllMember (const std::string& prefix,
                           const YAML::Node& node,
                           std::list<std::pair<std::string, const YAML::Node> >& output) {
    // Check
    if (prefix.find_first_not_of ("abcdefghijklmnopqrstuvwxyzABCDEFGHIMNOPQRSTUVWXYZ._0123456789")!=std::string::npos) {
        SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
        return;
    }
    // push pair of string and node
    output.push_back(std::make_pair(prefix, node));

    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            // recursive
            ListAllMember(prefix.empty() 
                          ? it->first.Scalar() // no prefix
                          : prefix + "." + it->first.Scalar(), // add prefix 
                          it->second, 
                          output);
        }
    }
    /*
    else if (node.IsSequence()) { 
        // travel through the sequence
        // casue bugs in logger
        for (size_t i = 0; i != node.size(); ++i) {
            ListAllMember(prefix.empty() ? node[i].Scalar()
                                         : prefix + node[i].Scalar(), node[i], output);
        }
    }
    */
}

void Config::LoadFromYaml(const YAML::Node& root) {
    // <name, node> pair
    std::list<std::pair<std::string, const YAML::Node> > all_nodes;
    // read nodes from yaml file
    ListAllMember ("", root, all_nodes);

    for (auto& i : all_nodes) {
        std::string key = i.first;
        if (key.empty()) {
            continue;
        }
        // transform to lowercase
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        // find 
        ConfigVarBase::ptr var = LookupBase(key);
        if (var) { // exist
            if (i.second.IsScalar()) {
                var->fromString(i.second.Scalar());
            }
            else{ // if (i.second.IsMap()){
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
}


void Config::Visit(std::function<void(ConfigVarBase::ptr)> callback) {
    MutexType::ReadLock lock(GetMutex());
    ConfigVarMap& m = GetData();
    // for customized operations on data
    for (auto it = m.begin();
         it != m.end(); ++it) {
            callback(it->second);
         }
}

}