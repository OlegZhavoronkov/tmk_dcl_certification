#pragma once
#ifndef __SERVERSETTINGS_H__
#define __SERVERSETTINGS_H__


#include <libcommon/server_settings_c.h>
#include <nlohmann/json_fwd.hpp>
#include <libcommon/IJsonSaveable.h>

namespace tmk
{
namespace common
{
namespace configuration
{

class ServerSettings: public ::server_settings,public IJsonSaveable
{
public:
    ServerSettings();
    ServerSettings(nlohmann::json& json);
    ServerSettings( const std::string& address,
                    unsigned int port,
                    int command_queue_size,
                    const std::string&  shmem_data_name,
                    size_t shmem_data_size,
                    const std::string&  boost_segment_name,
                    size_t minimal_boost_size);
    ServerSettings(const ServerSettings& other);                    
    ServerSettings(ServerSettings&& other);    
    ServerSettings& operator=(const ServerSettings& other);
    ServerSettings& operator=(ServerSettings&& other);
    bool operator==(const ServerSettings& other)const;                    
public:
    const std::string& getAddress() const;
    int getPort() const;
    int getCommandQueueSize() const;
    const std::string& getShmemDataName() const;
    size_t getShmemDataSize() const;
    const std::string& getBoostSegmentName() const;
    size_t getMinimalBoostSize() const;
public:
    
    constexpr static const char* configType="server_settings";
public:
    void fromJson(nlohmann::json& node) override;
    nlohmann::json toJson() const override;    
    void save(nlohmann::json& node) override;
private:
    std::string _address;
    unsigned int _port;
    int _command_queue_size;        //COMMAND_QUEUE_SIZE
    std::string _shmem_data_name;   //SHMEM_DATA_NAME_ENV
    size_t _shmem_data_size;        //SHMEM_DATA_SIZE_ENV;
    std::string _boost_segment_name;//BOOST_SEGMENT_NAME_ENV
    size_t _minimal_boost_size;     //MINIMAL_BOOST_SIZE;
private:
    void replicate();
    void copyFrom(const ServerSettings& other);
    void moveFrom(ServerSettings&& other);
};






} //namespace configuration
} //namespace common
} // namespace tmk
#endif // __SERVERSETTINGS_H__