#include <unordered_map>
#include <nlohmann/json.hpp>
#include <libcommon/PipelineConfig.h>
#include <libcommon/config_exceptions.h>
#include <boost/format.hpp>
#include <libcommon/config_factory.h>

//#include <iostream>
//#include <iomanip>
namespace 
{
    bool operator<( const tmk::common::configuration::PipelineNodeConfig& left,
                    const tmk::common::configuration::PipelineNodeConfig& right)
    {
        return left.getId() < right.getId();
    }
}
namespace tmk
{

namespace common
{

namespace configuration
{

template<PipelineNodeType type> struct NodeDesc;


#define DECLARENODEDESC(x) template<> struct NodeDesc<x> \
{ \
    constexpr static std::string_view name = #x; \
}

DECLARENODEDESC(FIRST);
DECLARENODEDESC(LAST);
DECLARENODEDESC(MIDDLE);
DECLARENODEDESC(NO_BLOCK);

using NodeNameToTypeMap = std::unordered_map<std::string,PipelineNodeType> ;
using NodeTypeToNameMap = std::unordered_map<PipelineNodeType,std::string> ;

template<PipelineNodeType head,PipelineNodeType... nodeTypes> 
    struct NodeDescMapMakerHelper;

template<PipelineNodeType... nodeTypes> struct NodeDescMapMaker
{
    constexpr static NodeNameToTypeMap InitializeForward()
    {
        NodeNameToTypeMap map;
        return std::move(NodeDescMapMakerHelper<nodeTypes...>::InitializeInternal(map));
    }

    constexpr static NodeTypeToNameMap InitializeBackward()
    {
        NodeTypeToNameMap map;    
        return std::move(NodeDescMapMakerHelper<nodeTypes...>::InitializeInternal(map));
    }
};



template<PipelineNodeType head,PipelineNodeType... nodeTypes> struct NodeDescMapMakerHelper
{
    constexpr static NodeNameToTypeMap InitializeInternal(NodeNameToTypeMap& input)
    {
        input.insert(std::make_pair(NodeDesc<head>::name,head));
        input = NodeDescMapMakerHelper<nodeTypes...>::InitializeInternal(input);
        return std::move(input);
    }
    constexpr static NodeTypeToNameMap InitializeInternal(NodeTypeToNameMap& input)
    {
        input.insert(std::make_pair(head,NodeDesc<head>::name));
        input = NodeDescMapMakerHelper<nodeTypes...>::InitializeInternal(input);
        return std::move(input);
    }
};

template<PipelineNodeType head> struct NodeDescMapMakerHelper<head>
{
    constexpr static NodeNameToTypeMap InitializeInternal(NodeNameToTypeMap& input)
    {
        input.insert(std::make_pair(NodeDesc<head>::name,head));
        return std::move(input);
    }

    constexpr static NodeTypeToNameMap InitializeInternal(NodeTypeToNameMap& input)
    {
        input.insert(std::make_pair(head,NodeDesc<head>::name));
        return std::move(input);
    }
};

static NodeNameToTypeMap ForwardMap = 
        NodeDescMapMaker<   PipelineNodeType::FIRST,
                            PipelineNodeType::LAST,
                            PipelineNodeType::MIDDLE,
                            PipelineNodeType::NO_BLOCK>::InitializeForward();

static NodeTypeToNameMap BackwardMap = 
        NodeDescMapMaker<   PipelineNodeType::FIRST,
                            PipelineNodeType::LAST,
                            PipelineNodeType::MIDDLE,
                            PipelineNodeType::NO_BLOCK>::InitializeBackward();



PipelineNodeConfig::PipelineNodeConfig(     int id,
                                            const std::string& path,
                                            const std::list<std::string>& launchParameters,
                                            PipelineNodeType nodeType, 
                                            int nextId,
                                            const std::string& nodeName) :
                                                _id(id),
                                                _path(path),
                                                _nodeType( nodeType ),
                                                _nodeName( nodeName ),
                                                _nextId( nextId )
{
    std::ranges::copy(launchParameters,std::back_inserter(_launchParameters) );
}


PipelineNodeConfig::PipelineNodeConfig(const std::string& blockName) :  
        _id(-1),
        _path(),
        _nodeType( PipelineNodeType::NO_BLOCK ),
        _nodeName( blockName ),
        _nextId( -1 )
{

}

void PipelineNodeConfig::fromJson(nlohmann::json& node) 
{
    try
    {
        _id = node["id"].get<int>();
        _path = node["name"].get<std::string>();
        _nodeType = ForwardMap.at(node[ "type" ].get<std::string>()) ;
        _nextId = node["next_id"].get<int>();
        auto parameters = node["parameters"];
        if(!parameters.is_array())
        {
            throw std::logic_error("parameters node doesnt hold an array!");                
        }
        std::list<std::string> pars;
        std::transform( parameters.begin(),parameters.end(),
                                std::back_inserter(pars),
                                [&](nlohmann::json& obj)->std::string
                                { 
                                    return obj.get<std::string>();
                                });
        _launchParameters.swap( pars );                                
    }
    catch(const std::exception& ex)
    {
        throw bad_config_exception((boost::format("%s:%d while parsing config exception was caught:%s") 
                                    % __FILE__ 
                                    % __LINE__ 
                                    % ex.what()).str());
    }
    
}

nlohmann::json PipelineNodeConfig::toJson() const 
{
    nlohmann::json json;
    json["id"] = _id;
    json["name"] = _path;
    json["next_id"] = _nextId;
    json["type"] = BackwardMap.at(_nodeType);
    json["parameters"] = _launchParameters;
    return json;
}

void PipelineNodeConfig::save(nlohmann::json& node) 
{
    node[_nodeName] = toJson();
}

const int PipelineNodeConfig::getId() const
{
    return _id;    
}

const std::string& PipelineNodeConfig::getPath() const
{
    return _path;
}

const std::list<std::string>& PipelineNodeConfig::getLaunchParameters() const
{
    return _launchParameters;
}

const PipelineNodeType PipelineNodeConfig::getNodeType() const
{
    return _nodeType;
}

const int PipelineNodeConfig::getNextId() const
{
    return _nextId;
}

const std::string& PipelineNodeConfig::getNodeName() const
{
    return _nodeName;
}



PipelineConfig::PipelineConfig(nlohmann::json& json) 
{
    PipelineConfig::fromJson( json) ;                 
}

PipelineConfig::PipelineConfig() 
{
    
}

PipelineConfig::iterator PipelineConfig::AddNode(const PipelineNodeConfig& node) 
{
    auto found_it= std::ranges::find_if(    _nodes,
                                            [&](const auto& existing)->bool
                                            {
                                                return existing.getId() == 
                                                    node.getId();
                                            });
    if(found_it != _nodes.end())
    {
        throw bad_config_exception((boost::format("%s:%d while adding node [%s,%d] \
                                                  already exisiting node with such id found [%s,%d]")
                                    % __FILE__
                                    % __LINE__
                                    % node.getNodeName().c_str()
                                    % node.getId()
                                    % found_it->getNodeName().c_str()
                                    % found_it->getId()).str()
                                );
    }
    _nodes.push_back(node);
    return std::ranges::find_if(    _nodes,
                                    [&](const auto& existing)->bool
                                        {
                                            return existing.getId() == 
                                                node.getId();});
}

PipelineConfig::size_type PipelineConfig::size() const
{
    return _nodes.size();
}

void PipelineConfig::fromJson(nlohmann::json& json) 
{
    std::vector<PipelineNodeConfig>().swap(_nodes);
    for(auto it = json.begin() ;it!= json.end() ;it++)
    {
        auto key = it.key();
        auto val = it.value();
        if( ! val.is_null() && val.is_object())
        {
            PipelineNodeConfig node( key );
            node.fromJson( val );
            _nodes.push_back( node );                
        }
    }
    if(!_nodes.empty())
    {
        std::stable_sort(_nodes.begin() , _nodes.end());
        const auto[min_elem ,max_elem] =std::minmax_element(_nodes.begin(),_nodes.end());
        if(min_elem->getId() != 0 || (max_elem->getId() - min_elem->getId()+1)!= _nodes.size() )
        {
            throw bad_config_exception((boost::format(
                                    "%s:%d min id %d ,max id %d, \
                                    _nodes.size() %d,violates corresponding lengths") 
                                                % __FILE__
                                                % __LINE__ 
                                                % min_elem->getId()
                                                % max_elem->getId()
                                                % _nodes.size()).str());
        }
        if(max_elem->getNextId()!= -1)
        {
            throw bad_config_exception((boost::format(
                                            "%s:%d max node with id %d and name \
                                            %s has next_id %d != -1") 
                                                % __FILE__
                                                % __LINE__ 
                                                % max_elem->getId()
                                                % max_elem->getNodeName()
                                                % max_elem->getNextId()).str());
        }                  
    }
    
}

nlohmann::json PipelineConfig::toJson() const 
{
    nlohmann::json json;
    if(!_nodes.empty())
    {
        if(_nodes.size() > 1)
        {
            std::stable_sort(_nodes.begin() , _nodes.end());
        }
        std::ranges::for_each(  _nodes,
                            [&](const auto& node)
                            {
                                nlohmann::json node_json = node.toJson();
                                json[node.getNodeName()] = node_json;          
                            }  );
    }
    return json;                            
}

void PipelineConfig::save(nlohmann::json& node) 
{
    _fabrics.ToJson(node,const_cast<const PipelineConfig&>(*this),_pipeLineConfigName);
}

PipelineNodeConfig& PipelineConfig::operator[](size_type idx) const
{
    return _nodes.at(idx);    
}


bool PipelineNodeConfig::operator==(const PipelineNodeConfig& other) const
{
    bool prelim=
            this->_id == other._id &&
            this->_nextId == other._nextId &&
            this->_nodeName == other._nodeName &&
            this->_nodeType == other._nodeType && 
            this->_path == other._path &&
            this->_launchParameters.size() == other._launchParameters.size();
    if(prelim)
    {
        return 
            std::ranges::all_of(this->_launchParameters,
                            [&](auto& str)->bool
                            {
                                return std::ranges::any_of(
                                        other._launchParameters,
                                        [&](auto& other_s)->bool
                                            {return other_s == str; });                                    
                            });
    }   
    return prelim;         
}


std::string& PipelineConfig::PipelineConfigName() 
{
    return _pipeLineConfigName;    
}

std::string& PipelineConfig::NodeId() 
{
    return PipelineConfig::PipelineConfigName();
}


bool PipelineConfig::operator==(const PipelineConfig& other) const
{
    bool intermediate = 
            this->_pipeLineConfigName == other._pipeLineConfigName && 
            this->_nodes.size() == other.size();
    if(!intermediate)
    {
        return intermediate;
    }            
    return std::ranges::all_of(_nodes,
                [&](auto& node)->bool
                {
                    auto first_it= std::ranges::find_if(other._nodes,
                                                [&](auto& node1)->bool
                                                {
                                                    return node1.getId() == node.getId();
                                                });
                    return first_it == other._nodes.end() ? false : *first_it == node;
                });
}


} //configuration
} //namespace common
} // namespace tmk