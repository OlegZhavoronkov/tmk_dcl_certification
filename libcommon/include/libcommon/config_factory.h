#pragma once
#include <type_traits>
#include <variant>
#include <mutex>
#include <functional>
#include <boost/format.hpp>
#include <nlohmann/json.hpp>
#include <map>
#include <libcommon/IJsonSaveable.h>
#include <libcommon/meta_info.h>
#include <libcommon/ServerSettings.h>
#include <libcommon/PipelineConfig.h>
#include <libcommon/FakeCameraConfig.h>
namespace tmk
{

namespace common
{

namespace configuration
{

template<typename ConfigElement,
            typename enabled=std::enable_if_t<std::is_constructible_v<
                                                std::string,
                                                decltype(ConfigElement::configType) > 
                                            > 
        > struct nameWrapper
{
    using ConfigElementType = ConfigElement;
    constexpr static const char* serializedName = ConfigElement::configType;
};

template<template<typename> class Wrapper,typename T,typename...Tail> struct MakeTL;

template<template<typename> class Wrapper,typename T> struct MakeTL<Wrapper,T>
{
    constexpr static const size_t size = 1;
    using tupletype = std::tuple<Wrapper<T> >;
    template<typename invokable,size_t sizeInd = 0 > 
        static bool apply(invokable& functor,size_t& idx)
    {
        idx = sizeInd == 0 ? 0 : idx;
        return functor( (std::add_pointer_t< Wrapper<T> >)nullptr );
    }
    template<typename invokable,typename Ret,size_t sizeInd = 0 > 
        static bool apply(invokable& functor,size_t& idx,Ret& ret)
    {
        idx = sizeInd == 0 ? 0 : idx;
        return functor( (std::add_pointer_t< Wrapper<T> >)nullptr,ret );
    }
};

template<template<typename> class Wrapper,typename T,typename...Tail> struct MakeTL
{
    
    using nextNodeType = MakeTL<Wrapper,Tail...>;
    constexpr static const size_t size = 1+nextNodeType::size;
    using currTupleType = std::tuple<Wrapper<T> >;
    using tupletype = decltype(std::tuple_cat<  currTupleType,
                                                typename nextNodeType::tupletype>
                                    (   std::declval<currTupleType>(), 
                                        std::declval<typename nextNodeType::tupletype >()
                                    ) );
    template<typename invokable,size_t sizeInd = 0> 
        static bool apply(invokable& functor,size_t& idx)
    {
        idx = sizeInd == 0 ? 0 : idx;
        if(functor( std::add_pointer_t<Wrapper<T> >(nullptr) ))
        {
            return true;
        }
        idx++;
        return nextNodeType::template apply<invokable,sizeInd == 0 ? 
            size : sizeInd >(functor,idx);
    }
    template<typename invokable,typename Ret,size_t sizeInd = 0> 
        static bool apply(invokable& functor,size_t& idx,Ret& ret)
    {
        idx = sizeInd == 0 ? 0 : idx;
        if(functor( std::add_pointer_t<Wrapper<T> >(nullptr) ,ret))
        {
            return true;
        }
        idx++;
        return nextNodeType::template apply<invokable,Ret,sizeInd == 0 ? 
            size : sizeInd >(functor,idx,ret);
    }
};

template<typename T, typename... Types> struct  TLIndex;

template<typename T, typename... Types> struct  TLIndex<T,T,Types...> : 
    std::integral_constant<size_t,0>{};
template<typename T, typename U,typename... Types> struct  TLIndex<T,U,Types...> : 
    std::integral_constant<size_t,TLIndex<T,Types...>::value +1 >{};
template<typename T, typename... Types> constexpr size_t TLIndex_v = 
        TLIndex<T,Types...>::value;

template<typename T,typename = void> struct HasCtorFabric : std::false_type{};
template<typename T> struct HasCtorFabric<  T , 
                                            std::enable_if_t<
                                                std::is_constructible_v<
                                                    T,
                                                    std::add_lvalue_reference_t<
                                                        nlohmann::json         > 
                                                                        > 
                                                            > 
                                        > 
                    : std::true_type{};

template<typename T,typename = void> struct HasStaticFabric : std::false_type{};
template<typename T > struct HasStaticFabric<
                                T , 
                                std::enable_if_t<
                                    std::is_invocable_r_v<
                                        T,
                                        decltype(T::create),
                                        std::add_lvalue_reference_t<nlohmann::json> 
                                                        > 
                                                > 
                                            > : std::true_type{};

template<typename T> constexpr bool HasCtorFabric_v  = HasCtorFabric<T>::value;
template<typename T> constexpr bool HasStaticFabric_v  = HasStaticFabric<T>::value;


template<typename ConfigElement,typename =void> struct ConfigFabricEntry;
template<typename ConfigElement > 
struct ConfigFabricEntry<ConfigElement,
                        std::enable_if_t<std::is_constructible_v<
                                            std::string,
                                            decltype(ConfigElement::configType) 
                                                                > &&
                                        ! HasCtorFabric_v<ConfigElement> 
                                        && HasStaticFabric_v<ConfigElement> 
                                        > 
                        >
{

    constexpr static const char* name=ConfigElement::configType;
    static std::function<void(nlohmann::json& json)> getCreator(ConfigElement& blank)
    {
        return 
        ([&](nlohmann::json& json)
                                        {
                                            blank = ConfigElement::create( json );
                                        });
    }

    static void PopulateMap( std::map<    std::string,
                                            std::function<void(nlohmann::json&)>
                                        >& map, 
                                ConfigElement& blanc)
    {
        map.insert(std::make_pair(  std::string(name), 
                                    std::function<void(nlohmann::json& json)>
                                    (getCreator( blanc )) )
                  );
    }
};

template<typename ConfigElement > 
struct ConfigFabricEntry<ConfigElement,
                        std::enable_if_t<   std::is_constructible_v<
                                                std::string,
                                                decltype(ConfigElement::configType) 
                                                                    > 
                                            && HasCtorFabric_v<ConfigElement> 
                                            && !HasStaticFabric_v<ConfigElement> 
                                        > 
                        >
{

    constexpr static const char* name=ConfigElement::configType;
    static std::function<void(nlohmann::json& json)> getCreator(ConfigElement& blank)
    {
        return 
        ([&](nlohmann::json& json)
                                        {
                                            blank = ConfigElement( json );
                                        });
    }

    static void PopulateMap( std::map<    std::string,
                                            std::function<void(nlohmann::json&)>
                                        >& map, 
                                ConfigElement& blanc)
    {
        map.insert(std::make_pair(  std::string(name), 
                                    std::function<void(nlohmann::json& json)>
                                    (getCreator(blanc)) )
                  );
    }
    
};

    template<   typename T,
                typename = void
            >   struct setName;


    template<   typename T
            >   struct setName< T,
                                std::enable_if_t< 
                                    std::is_base_of_v <IJsonNodeId ,T> 
                                                > 
                                > 
    {
        static inline void name(T& t,const std::string& key)
        {
            static_cast<IJsonNodeId*>(&t)->NodeId() = key;                
        }        
    };
    
    template<   typename T>   struct setName< T,
                                    std::enable_if_t< 
                                        ! std::is_base_of_v<IJsonNodeId  ,T> 
                                                    > 
                                            > 
    {
        static inline void name(T& t,const std::string& key)
        {

        }    
    };


struct FabricsNames
{
    constexpr static const char* typeAttr = "serialized_as";
    constexpr static const char* nameAttr = "object_name";
    constexpr static const char* defaultNameAttr="default";
};

template<typename... serializedTypes> class Fabrics:public FabricsNames
{
public:
    
    Fabrics()
    {
        visitor<serializedTypes...>::visit(_creators,_blank);
        std::ranges::transform( _creators,
                                std::back_inserter( _registeredSerializedHelpers),
                                                    [](const auto& kvp)->auto
                                                    { return kvp.first; });
        std::ranges::remove_if(_registeredSerializedHelpers,
                                [](const auto& st)->bool
                                {return st == meta_info::configType;});
    }

    Fabrics(const Fabrics&) = delete;
    Fabrics& operator= (const Fabrics&) = delete;
    Fabrics(Fabrics&& other)
    {
        std::scoped_lock lock(other._creationMTX);
        _creators = std::move( other._creators );
    }

    template<typename ConfigToCreate,
            typename registered= 
                std::enable_if_t<   
                    std::disjunction_v <
                        std::is_same<ConfigToCreate,serializedTypes>... 
                                        >  
                                >  
            > 
    ConfigToCreate Create(nlohmann::json& json)
    {
        std::scoped_lock lock( _creationMTX  );
        const auto typeName = json[typeAttr].get<std::string>();
        if( !json[typeAttr].is_null())
        {
            auto it= _creators.find(typeName);
            if(it != _creators.end() )
            {
                (*it).second(json);
                return std::get< TLIndex_v<
                                    ConfigToCreate,
                                    serializedTypes...
                                        > 
                                >(_blank);
            }   
            else
            {
                throw std::logic_error(
                    (boost::format("fail to create element - no creator for type string  %s") 
                        % typeName ).str() );        
            }             
        }
        throw std::logic_error((boost::format("fail to create element in %s") % __FUNCTION__ ).str());
    }

    template<typename ConfigToSave,
            typename registered= std::enable_if_t<
                                    std::disjunction_v <
                                        std::is_same<ConfigToSave,serializedTypes>...>  
                                        && std::is_assignable_v<
                                                IJsonSaveable&,
                                                ConfigToSave& > 
                                                >  
            > 
    void ToJson(    nlohmann::json& Json,
                    const ConfigToSave& toSave,
                    const std::string& objectName)
    {
        constexpr const auto* typeName= ConfigFabricEntry<ConfigToSave>::name;
        auto objJson = static_cast<const IJsonSaveable&>(toSave).toJson();
        objJson[typeAttr] = typeName;
        objJson[nameAttr] = objectName.empty() ? defaultNameAttr : objectName;
        Json[objectName.empty() ? typeName : objectName]=objJson;
    }

    const std::list<std::string>& getRegisteredSerializedStrings()const
    {
        return _registeredSerializedHelpers;
    }

    constexpr size_t getTypeIdx(const std::string& typeTag)const
    {
        using tl= MakeTL< nameWrapper , serializedTypes...>;
        size_t idx;
        auto lambda = [&]<typename lambdaArg>(lambdaArg*)->bool
        {
            return lambdaArg::serializedName == typeTag;
        };
        if(tl::apply(lambda,idx))
        {
            return idx;
        }
        throw std::logic_error("type is absent");
    }

    template<typename T> constexpr  auto getTypeIdx()
    {
        using tl= MakeTL< nameWrapper , serializedTypes...>;
        constexpr size_t idx = TLIndex_v< nameWrapper<T> , tl>;
        return std::make_tuple(idx,std::string(nameWrapper<T>::serializedName));
    } 
private:

public:
    std::shared_ptr<IJsonSaveable> 
        CreateFromElementByTypeName(const std::string& serialized_tag,
                                    const std::string& key,
                                    nlohmann::json& json)
    {
        using tl= MakeTL< nameWrapper , serializedTypes...>;
        size_t idx;
        std::shared_ptr<IJsonSaveable> retPtr;
        auto lambda = 
        [&]<typename lambdaArg,typename retPtrType>(lambdaArg*,retPtrType& ret)->bool
        {
            using returnType = typename lambdaArg::ConfigElementType;
            if( lambdaArg::serializedName == serialized_tag )
            {
                returnType createdObject=
                    Create<typename lambdaArg::ConfigElementType>(json);
                setName<returnType>::name(createdObject,key);
                ret.reset(new returnType(createdObject));
                return true;
            }
            return false;
        };
        if(tl::template apply(lambda,idx,retPtr))
        {
            return retPtr;
        }
        throw std::logic_error("type is absent");
        
    }

public:
    typedef std::tuple<serializedTypes...> blankType;
    blankType _blank;
private:
    std::mutex _creationMTX;

    std::map<std::string,std::function<void(nlohmann::json&)> > _creators;
    std::list<std::string> _registeredSerializedHelpers;
    template<typename T,typename... Tail> struct visitor;
    template<typename T > struct visitor<T>
    {
        using configType = ConfigFabricEntry<T>;
        static void visit(  std::map<   std::string,
                                        std::function<void(nlohmann::json&)> 
                                    >& map, 
                            blankType& blank)
        {
            configType::PopulateMap(    map, 
                                            std::get< 
                                                TLIndex_v<T,serializedTypes...> 
                                                    >(blank));
        }
        
    };

    template<typename T,typename... Tail> struct visitor
    {
        using configType = ConfigFabricEntry<T>;
        static void visit(  std::map<   std::string,
                                        std::function<void(nlohmann::json&)> 
                                    >& map,
                            blankType& blank)
        {    
            configType::PopulateMap(    map, 
                                            std::get< 
                                                TLIndex_v<T,serializedTypes...> 
                                                    >(blank));
            visitor<Tail...>::visit(map,blank);
        }
    };

};


typedef Fabrics< meta_info,ServerSettings,PipelineConfig,FakeCameraConfig > 
        TheFabrics;

extern TheFabrics _fabrics;


} //namespace configuration
} //namespace common
} // namespace tmk