#include <nlohmann/json.hpp>
#include <libcommon/ServerSettings.h>
#include <libcommon/config_factory.h>

namespace tmk
{
namespace common
{
namespace configuration
{


ServerSettings::ServerSettings() 
{
    _address="127.0.0.1";
    _port = 4444;
    _command_queue_size=20;//COMMAND_QUEUE_SIZE
    _shmem_data_name = "SHMEM_DATA_NAME";//SHMEM_DATA_NAME_ENV
    _shmem_data_size;//SHMEM_DATA_SIZE_ENV;//SHMEM_DATA_SIZE_ENV
    _boost_segment_name="BOOST_SEGMENT_NAME";//BOOST_SEGMENT_NAME_ENV
    _minimal_boost_size=2000000000;//MINIMAL_BOOST_SIZE;
    _shmem_data_size = _minimal_boost_size;
    replicate();
}

ServerSettings::ServerSettings( const std::string& address,
                                unsigned int port,
                                int command_queue_size,
                                const std::string&  shmem_data_name,
                                size_t shmem_data_size,
                                const std::string&  boost_segment_name,
                                size_t minimal_boost_size) :    
                                        _address(address),
                                        _port(port),
                                        _command_queue_size(command_queue_size),
                                        _shmem_data_name(shmem_data_name),
                                        _shmem_data_size(shmem_data_size),
                                        _boost_segment_name(boost_segment_name),
                                        _minimal_boost_size(minimal_boost_size)
{
    replicate();
}


bool ServerSettings::operator==(const ServerSettings& other) const
{
    return  _address==other._address &&
            _port==other._port &&
            _command_queue_size==other._command_queue_size &&
            _shmem_data_name==other._shmem_data_name &&
            _shmem_data_size==other._shmem_data_size &&
            _boost_segment_name==other._boost_segment_name &&
            _minimal_boost_size==other._minimal_boost_size;
}


ServerSettings::ServerSettings(nlohmann::json& json) 
{
    ServerSettings::fromJson( json );
}

ServerSettings::ServerSettings(const ServerSettings& other) 
{
    copyFrom(other);    
}

ServerSettings::ServerSettings(ServerSettings&& other) 
{
    moveFrom(std::forward<ServerSettings&&>(other));
}

ServerSettings& ServerSettings::operator=(const ServerSettings& other) 
{
    copyFrom(other);
    return *this;
}

ServerSettings& ServerSettings::operator=(ServerSettings&& other) 
{
    moveFrom(std::forward<ServerSettings&&>(other));
    return *this;
}

void ServerSettings::copyFrom(const ServerSettings& other) 
{
    _address=other._address ;
    _port=other._port ;
    _command_queue_size=other._command_queue_size ;
    _shmem_data_name=other._shmem_data_name ;
    _shmem_data_size=other._shmem_data_size ;
    _boost_segment_name=other._boost_segment_name ;
    _minimal_boost_size=other._minimal_boost_size;
    replicate();
}

void ServerSettings::moveFrom(ServerSettings&& other) 
{
    std::swap(_address,other._address );
    std::swap(_port,other._port );
    std::swap(_command_queue_size,other._command_queue_size );
    std::swap(_shmem_data_name,other._shmem_data_name );
    std::swap(_shmem_data_size,other._shmem_data_size );
    std::swap(_boost_segment_name,other._boost_segment_name );
    std::swap(_minimal_boost_size,other._minimal_boost_size);
    replicate();
}

void ServerSettings::replicate() 
{
    auto* pRaw = static_cast<::server_settings*>(this);
    pRaw->address = _address.c_str();
    pRaw->port = _port;
    pRaw->command_queue_size = _command_queue_size;//COMMAND_QUEUE_SIZE
    pRaw->shmem_data_name = _shmem_data_name.c_str();//SHMEM_DATA_NAME_ENV
    pRaw->shmem_data_size_env = _shmem_data_size;//SHMEM_DATA_SIZE_ENV;//SHMEM_DATA_SIZE_ENV
    pRaw->boost_segment_name_env = _boost_segment_name.c_str();//BOOST_SEGMENT_NAME_ENV
    pRaw->minimal_boost_size = _minimal_boost_size;//MINIMAL_BOOST_SIZE;
    pRaw->shmem_data_size_env = _shmem_data_size;
}


void ServerSettings::fromJson(nlohmann::json& node) 
{
    _address = node["address"];
    _port = node["port"];
    _command_queue_size = node["command_queue_size"];
    _shmem_data_name = node["shmem_data_name"];
    _shmem_data_size = node["shmem_data_size"];
    _boost_segment_name = node["boost_segment_name"];
    _minimal_boost_size = node["minimal_boost_size"];
    _shmem_data_size = node["shmem_data_size"];
    replicate();
}


nlohmann::json ServerSettings::toJson() const 
{
    nlohmann::json node;
    node["address"] = _address ;
    node["port"] = _port ;
    node["command_queue_size"]=_command_queue_size;
    node["shmem_data_name"] = _shmem_data_name ;
    
    node["shmem_data_size"] = _shmem_data_size;
    node["boost_segment_name"] = _boost_segment_name;
    node["minimal_boost_size"] = _minimal_boost_size ;
    node["shmem_data_size"]=_shmem_data_size;
    return std::move(node);
}


void ServerSettings::save(nlohmann::json& node) 
{
    _fabrics.ToJson(node,const_cast<const ServerSettings&>(*this),std::string());
}

const std::string& ServerSettings::getAddress() const
{
    return _address;    
}

int ServerSettings::getPort() const
{
    return _port;
}

int ServerSettings::getCommandQueueSize() const
{
    return _command_queue_size;
}

const std::string& ServerSettings::getShmemDataName() const
{
    return _shmem_data_name;
}

size_t ServerSettings::getShmemDataSize() const
{
    return _shmem_data_size;
}

const std::string& ServerSettings::getBoostSegmentName() const
{
    return _boost_segment_name;
}

size_t ServerSettings::getMinimalBoostSize() const
{
    return _minimal_boost_size;
}



} //configuration
} //namespace common
} // namespace tmk