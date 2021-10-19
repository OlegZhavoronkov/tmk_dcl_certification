#include <libcommon/meta_info.h>
#include <libcommon/time_utils.h>
#include <nlohmann/json.hpp>
#include <nlohmann/detail/conversions/from_json.hpp>
#include <libcommon/config_factory.h>
namespace tmk
{
namespace common
{

namespace configuration
{

meta_info::meta_info() : _creationTime( clock::now( ) )    
{
    _lastModificationTime = _creationTime;
    _readableCreationTime = details::TimePointToString(_creationTime);
    _readableModificationTime = details::TimePointToString(_lastModificationTime);
}

void meta_info::touch()
{
    _lastModificationTime = clock::now();
    _readableModificationTime = details::TimePointToString( _lastModificationTime );
}


const meta_info::time_point& meta_info::getCreationTime() const
{
    return _creationTime;
}


const meta_info::time_point& meta_info::getLastModificationTime() const
{
    return _lastModificationTime;
}

const std::string& meta_info::getComment() const
{
    return _comment;
}

meta_info meta_info::create(nlohmann::json& json) 
{
    return meta_info(json);
}


meta_info::meta_info(nlohmann::json& node) 
{
    meta_info::fromJson(node);
}

void meta_info::fromJson(nlohmann::json& node) 
{
    auto tsec= node["creationTime"]["sec"].get<long int>();
    auto nsec = node["creationTime"]["nsec"].get<long>();
    _creationTime = clock::from_time_t(std::time_t(tsec))+clock::duration(nsec);
    _readableCreationTime = details::TimePointToString(_creationTime);
    tsec= node["lastModificationTime"]["sec"].get<long int>();
    nsec = node["lastModificationTime"]["nsec"].get<long>();
    _lastModificationTime = clock::from_time_t(std::time_t(tsec))+clock::duration(nsec);
    _readableModificationTime = details::TimePointToString( _lastModificationTime );
    _comment=node["comment"].get<std::string>();
}

nlohmann::json meta_info::toJson() const 
{
    nlohmann::json node;
    std::timespec ts = details::TimePointToTimeSpec(_creationTime);
    node["creationTime"]["sec"] = ts.tv_sec;
    node["creationTime"]["nsec"] = ts.tv_nsec;
    node["creationTime"]["readable"] = _readableCreationTime;
    ts = details::TimePointToTimeSpec(_lastModificationTime);
    node["lastModificationTime"]["sec"] = ts.tv_sec;
    node["lastModificationTime"]["nsec"] = ts.tv_nsec;
    node["lastModificationTime"]["readable"] = _readableModificationTime;
    node["comment"] = _comment;
    return node;
}


void meta_info::save(nlohmann::json& node) 
{
    _fabrics.ToJson(node,const_cast<const meta_info&>(*this),std::string());
}


bool meta_info::operator==(const meta_info& other) const
{
    return  _creationTime == other._creationTime && 
            _lastModificationTime == other._lastModificationTime &&
            _comment == other._comment;

}


} //configuration
} //namespace common
} // namespace tmk