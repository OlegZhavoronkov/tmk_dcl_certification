#pragma once
#ifndef __META_INFO_H__
#define __META_INFO_H__


#include <chrono>
#include <string>
#include <nlohmann/json_fwd.hpp>
#include <libcommon/IJsonSaveable.h>
namespace tmk
{

namespace common
{

namespace configuration
{

class meta_info:public IJsonSaveable
{
public:
    using clock = std::chrono::system_clock;
    using time_point = std::chrono::time_point<clock>;
public:
    meta_info();
    meta_info(const meta_info&) = default;
    meta_info& operator=(const meta_info&) = default;
    void touch();
    const time_point& getCreationTime()const;
    const time_point& getLastModificationTime()const;
    const std::string& getComment( ) const;   
    static meta_info create(nlohmann::json& json);
    constexpr static const char* configType="meta_info";
    bool operator==(const meta_info& other)const;
private:
    meta_info(nlohmann::json& node);
public:
    void fromJson(nlohmann::json& node) override;
    nlohmann::json toJson() const override;
    void save(nlohmann::json& node) override;
private:
    time_point _creationTime;
    time_point _lastModificationTime;
    std::string _comment = std::string();
    std::string _readableCreationTime;
    std::string _readableModificationTime;
};



} //namespace configuration
} //namespace common
} // namespace tmk
#endif // __META_INFO_H__