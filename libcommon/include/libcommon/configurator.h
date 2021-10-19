#pragma once
#ifndef __CONFIGURATOR_H__
#define __CONFIGURATOR_H__


#include <memory>
#include <string>
#include <nlohmann/json_fwd.hpp>
#include <libcommon/meta_info.h>
#include <variant>
#include <unordered_map>
namespace tmk
{

namespace common
{

namespace configuration
{

struct ConfigHolderKeyHash;
class ConfigHolderKey
{
    friend struct ConfigHolderKeyHash;
public:
    ConfigHolderKey(const std::string& serializedTag,const std::string& nameTag);
    //TODO should be implemented
    /*template<typename T> static ConfigHolderKey ByType()
    {
        auto [idx, name] = _fabrics.getTypeIdx<T>();
    }*/
    bool operator== (const ConfigHolderKey& other)const;
private:    
    std::string _serializedTag;
    std::string _objectName;
    size_t _serializedIdx;
};

struct ConfigHolderKeyHash : std::hash<std::string>
{
    size_t operator()(const ConfigHolderKey& key)const;
};
class Configuration
{
public:
    Configuration(const Configuration&) = default;
    Configuration& operator=(const Configuration&) = default;
    static Configuration& Instance();
    static Configuration& Load(const std::string& pathToConfig);
    /*very dangerous operation which should be used with extreme care!!!*/
    static void Reset();
public:
    void Save(const std::string& pathToSave= std::string());
    const IJsonSaveable& getByTag(  const std::string& tag ,
                                    const std::string& name = std::string());
    //TODO should be redesigned due to complexity of config_fabrics which will involved from here
    /*template<typename T,
             typename compatible= std::enable_if_t<std::is_assignable_v<std::add_pointer_t<IJsonSaveable>,T*> > >
    const T& getByType(const std::string& name = std::string())
    {

    }*/
    template<typename...Types> Configuration(std::tuple<Types...>& tl)
    {
        auto functor=[&](auto& val)
        {
            using the_type = std::remove_reference_t<decltype(val)>;
            ConfigHolderKey hash(the_type::configType,std::string());
            std::shared_ptr<IJsonSaveable> ptr = 
                std::make_shared<the_type>(const_cast<const the_type&>(val));
            _storage.insert(std::make_pair(hash,ptr));                     
        };
        std::apply( [&](auto& ...x)
                    {
                        (...,functor(x));                            
                    },tl);               
    }
protected:
    explicit Configuration(const std::string& pathToConfig);
    
    static std::shared_ptr<Configuration> _pConfiguration;
private:
    std::string _path;
    meta_info _meta;
    using storageType= std::unordered_map<  ConfigHolderKey,
                                            std::shared_ptr<IJsonSaveable>,
                                            ConfigHolderKeyHash >;
    storageType _storage;
private:
    void parseConfig(nlohmann::json json);
};




} //namespace configuration
} //namespace common
} // namespace tmk
#endif // __CONFIGURATOR_H__