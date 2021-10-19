#include <nlohmann/json.hpp>
#include <libcommon/FakeCameraConfig.h>
#include <libcommon/config_factory.h>

namespace tmk
{
namespace common
{

namespace configuration
{
    
FakeCameraConfig::FakeCameraConfig() :  _listeningAddress("127.0.0.1"),
                                        _port(4444),
                                        _imagesPath("../test/Data")
{
    
}

FakeCameraConfig::FakeCameraConfig(nlohmann::json& json) 
{
    FakeCameraConfig::fromJson( json );
}

const std::string& FakeCameraConfig::getListeningAddress() const
{
    return _listeningAddress;
}

const unsigned int FakeCameraConfig::getPort() const
{
    return _port;
}

const std::string& FakeCameraConfig::getImagesPath() const
{
    return _imagesPath;
}

void FakeCameraConfig::fromJson(nlohmann::json& node) 
{
    _listeningAddress = node["address"].get<std::string>();
    _port=node["port"].get<unsigned int>();
    _imagesPath= node["images_path"].get<std::string>();
}

nlohmann::json FakeCameraConfig::toJson() const 
{
    nlohmann::json node;
    node["address"] = _listeningAddress;
    node["port"] = _port;
    node["images_path"] = _imagesPath;
    return node;
}

void FakeCameraConfig::save(nlohmann::json& node) 
{
    _fabrics.ToJson(node,const_cast<const FakeCameraConfig&>(*this),std::string() );
}


} //configuration
} //namespace common
} // namespace tmk