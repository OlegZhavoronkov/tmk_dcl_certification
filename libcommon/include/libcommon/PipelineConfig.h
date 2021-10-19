#pragma once
#ifndef __PIPELINECONFIG_H__
#define __PIPELINECONFIG_H__


#include <nlohmann/json_fwd.hpp>
#include <libcommon/IJsonSaveable.h>
#include <list>

namespace tmk
{
namespace common
{
namespace configuration
{

enum PipelineNodeType
{
    NO_BLOCK = -1,
    FIRST,
    MIDDLE,
    LAST
};

class PipelineNodeConfig :public IJsonSaveable
{
public:    
    PipelineNodeConfig(     int id, const std::string& path,
                            const std::list<std::string>& launchParameters,
                            PipelineNodeType nodeType, int nextId,
                            const std::string& nodeName);
    explicit PipelineNodeConfig(const std::string& blockName);                            

    bool operator==(const PipelineNodeConfig& other)const;
public:
    void fromJson(nlohmann::json& node) override;
    nlohmann::json toJson() const override;    
    void save(nlohmann::json& node) override;
public:
    const int getId()const;
    const std::string& getPath() const;
    const std::list<std::string>& getLaunchParameters() const;
    const PipelineNodeType getNodeType() const;
    const int getNextId() const;
    const std::string&  getNodeName() const;
private:
    int _id;
    std::string _path;
    std::list<std::string> _launchParameters;
    PipelineNodeType _nodeType;
    int _nextId;
    std::string _nodeName;
};



class PipelineConfig : public IJsonSaveable,public IJsonNodeId
{
public:
    PipelineConfig(nlohmann::json& json);
    PipelineConfig();

    constexpr static const char* configType="pipeline_settings";

public:
    using iterator = std::vector<PipelineNodeConfig>::iterator;
    using size_type = std::vector<PipelineNodeConfig>::size_type;
    iterator AddNode(const PipelineNodeConfig& node);
    PipelineNodeConfig& operator[](size_type idx) const;
    size_type size()const;
    std::string& PipelineConfigName();
    bool operator==(const PipelineConfig& other)const;
public:
    void fromJson(nlohmann::json& node) override;
    nlohmann::json toJson() const override;    
    void save(nlohmann::json& node) override;
public:
    std::string& NodeId() override;
private:
    mutable std::vector<PipelineNodeConfig> _nodes;
    std::string _pipeLineConfigName;
};


} //namespace configuration
} //namespace common
} // namespace tmk
#endif // __PIPELINECONFIG_H__