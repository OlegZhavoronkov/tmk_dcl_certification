#pragma once
#ifndef __FAKECAMERACONFIG_H__
#define __FAKECAMERACONFIG_H__


#include <nlohmann/json_fwd.hpp>
#include <libcommon/IJsonSaveable.h>

namespace tmk
{

namespace common
{

namespace configuration
{

class FakeCameraConfig:public IJsonSaveable
{
public:
    FakeCameraConfig();
    FakeCameraConfig(nlohmann::json& json);
    
    constexpr static const char* configType="fake_camera_settings";
public:
    const std::string& getListeningAddress() const;
    const unsigned int getPort() const;
    const std::string& getImagesPath() const;
public:
    void fromJson(nlohmann::json& node) override;
    nlohmann::json toJson() const override;    
    void save(nlohmann::json& node) override;
private:
    std::string _listeningAddress;
    unsigned int _port;
    std::string _imagesPath;

};



} //namespace configuration
} //namespace common
} // namespace tmk
#endif // __FAKECAMERACONFIG_H__