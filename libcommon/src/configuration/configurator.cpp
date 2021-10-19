#include <algorithm>
//#include <iostream>
#include <iomanip>
#include <libcommon/configurator.h>
#include <libcommon/config_factory.h>
#include <libcommon/meta_info.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <libcommon/config_exceptions.h>
#include <unordered_map>

namespace tmk
{
namespace common
{

namespace configuration
{


ConfigHolderKey::ConfigHolderKey(   const std::string& serializedTag,
                                    const std::string& nameTag) :   
                                        _serializedTag(serializedTag),
                                        _objectName(nameTag),
                                        _serializedIdx( 
                                                _fabrics.getTypeIdx(serializedTag) )
{
}


bool ConfigHolderKey::operator== (const ConfigHolderKey& other) const
{
    return  _serializedIdx == other._serializedIdx &&
            _objectName == other._objectName;
}


size_t ConfigHolderKeyHash::operator()(const ConfigHolderKey& key) const
{
    return ((!key._objectName.empty()? 
                (static_cast<const std::hash<std::string>&>(*this))(key._objectName) : 0) << 3)
                | key._serializedIdx;
}


std::shared_ptr<Configuration> Configuration::_pConfiguration 
    = std::shared_ptr<Configuration>();


Configuration& Configuration::Load(const std::string& pathToConfig) 
{
    auto pCurrent = _pConfiguration;
    if(pCurrent!=nullptr)
    {
        throw config_not_found_exception(pathToConfig);
    }
    try
    {
        _pConfiguration = std::shared_ptr<Configuration>
                            (new Configuration(pathToConfig) );
    }
    catch(const configuration_exception& ex)
    {
        throw;
    }
    catch(const std::exception& ex)
    {
        throw bad_config_exception(pathToConfig,std::string(ex.what()));
    }
    
    return *_pConfiguration;
}


Configuration& Configuration::Instance() 
{
    auto pCurrent = _pConfiguration;
    if(!pCurrent)
    {
        throw std::logic_error("current configuration must be non-empty");
    }
    return *pCurrent;
}


Configuration::Configuration(const std::string& pathToConfig) 
{
    
    nlohmann::json json;
    std::ifstream str(pathToConfig);
    if(!str.is_open() )
    {
        throw config_not_found_exception( pathToConfig );
    }
    str >> json;
    //bool meta_found = false;
    nlohmann::json meta_json;
    for(auto it= json.begin() ;it != json.end() ;it++)
    {
        auto serialized_as_tag = (*it)[FabricsNames::typeAttr];
        auto key= std::string(it.key());
        if( !serialized_as_tag.is_null()  
            && serialized_as_tag.get<std::string>() == meta_info::configType
            && key == meta_info::configType)
        {   
            meta_json = *it;
            break;
        }
    }
    if( meta_json.is_null() )
    {
        throw bad_config_exception(pathToConfig,"meta_info key not found");
    }
    _meta = meta_info::create(meta_json);
    parseConfig(json);
    
}


void Configuration::parseConfig(nlohmann::json json) 
{       
    std::unordered_map< ConfigHolderKey,
                        std::shared_ptr<IJsonSaveable>,
                        ConfigHolderKeyHash > configElementsMap;
    for(auto it= json.begin() ;it != json.end() ;it++)
    {
        auto serialized_as_tag = (*it)[FabricsNames::typeAttr];
        auto object_name_tag = (*it)[FabricsNames::nameAttr];
        auto key= std::string(it.key());
        if( !serialized_as_tag.is_null() )
        {   
            auto typeTag = serialized_as_tag.get<std::string>();
            if(std::ranges::any_of( _fabrics.getRegisteredSerializedStrings(),
                                    [&](const auto& name)->bool
                                        { return name == typeTag; } ) )
            {
                if( key == typeTag 
                    || (    !object_name_tag.is_null() 
                            && object_name_tag == FabricsNames::defaultNameAttr ))
                {
                    key = std::string();                        
                }
                ConfigHolderKey hash_key (typeTag,key);   
                auto json = it.value();
                auto ptr = _fabrics.CreateFromElementByTypeName(typeTag,key,json );                                                                    
                configElementsMap.insert(std::make_pair(hash_key, ptr) );                                                                
            }
        }
    }                 
    _storage.swap(configElementsMap)  ;
}

const IJsonSaveable& Configuration::getByTag(   const std::string& tag ,
                                                const std::string& name )
{
    ConfigHolderKey hash_key (tag,name);   
    auto ptr = _storage.at(hash_key);
    return *ptr;
}

void Configuration::Save(const std::string& pathToSave) 
{
    std::string pathToSaveReal = !pathToSave.empty() ? pathToSave : _path;
    if( pathToSaveReal.empty() )
    {
        throw bad_config_exception(_path,"path not specified");
    }
    _meta.touch();
    nlohmann::json json;
    _meta.save(json);
    std::ranges::for_each(  _storage,
                            [&](auto& kvp)
                            {
                                kvp.second->save(json);
                            });
    std::ofstream fs(pathToSaveReal,std::ios::binary | std::ios::trunc);
    fs << std::setw(4) << json;                                
}

void Configuration::Reset() 
{
    _pConfiguration= std::shared_ptr<Configuration>();
}

} //configuration
} //namespace common
} // namespace tmk