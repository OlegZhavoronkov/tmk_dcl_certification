#pragma once
#ifndef __IJSONSAVEABLE_H__
#define __IJSONSAVEABLE_H__

#include <nlohmann/json_fwd.hpp>
namespace tmk
{

namespace common
{

namespace configuration
{

struct ISaveable
{
    virtual void save(nlohmann::json& node) = 0;
    virtual ~ISaveable() = default;
};

struct IJsonSaveable: public ISaveable
{
    virtual void fromJson(nlohmann::json& node) = 0;
    virtual nlohmann::json toJson() const = 0;
    virtual ~IJsonSaveable() = default;
};

struct IJsonNodeId
{
    virtual std::string& NodeId() = 0;
    virtual ~IJsonNodeId() = default;
};


} //namespace configuration
} //namespace common
} // namespace tmk
#endif // __IJSONSAVEABLE_H__