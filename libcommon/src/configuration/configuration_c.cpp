#include <libcommon/configuration_c.h>
#include <libcommon/configurator.h>
#include <libcommon/config_exceptions.h>
#include <libcommon/ServerSettings.h>
#include <libcommon/PipelineConfig.h>

namespace tmk_config = tmk::common::configuration;


const struct server_settings * loadServerSettings(const char* configPath) noexcept
{
    try
    {
        tmk_config::Configuration::Load(std::string(configPath));
        const auto& serverConfig = 
            static_cast<const tmk_config::ServerSettings&>( 
                tmk_config::Configuration::Instance()
                                            .getByTag(tmk_config::ServerSettings::configType));
        return  (&serverConfig);
    }
    catch(tmk_config::configuration_exception& exception){
        
    }
    catch(const std::exception& stdex){

    }
    catch(...){

    }
    return NULL;
}
const struct server_settings * getServerSettings() noexcept
{
    try
    {
        const auto& serverConfig =  
            tmk_config::Configuration::Instance()
                                        .getByTag(tmk_config::ServerSettings::configType);
        return  (&static_cast<const tmk_config::ServerSettings&>(serverConfig));
    }
    catch(tmk_config::configuration_exception& exception){
        
    }
    catch(const std::exception& stdex){

    }
    catch(...){

    }
    return NULL;
}

void saveDefaultConfig(const char* configPath) noexcept
{
    tmk_config::ServerSettings copy;
    auto new_tuple_type=std::make_tuple(
        tmk_config::ServerSettings(),
        ([]()->tmk_config::PipelineConfig
        {
            tmk_config::PipelineConfig pipe_config;
            pipe_config.PipelineConfigName()="default";
            pipe_config.AddNode(    tmk_config::PipelineNodeConfig( 0,
                                                                    "",
                                                                    {},
                                                                    tmk_config::PipelineNodeType::FIRST,
                                                                    1,
                                                                    "first"));
            pipe_config.AddNode(    tmk_config::PipelineNodeConfig( 1,
                                                                    "",
                                                                    {},
                                                                    tmk_config::PipelineNodeType::LAST,
                                                                    -1,
                                                                    "last"));
            return std::move(pipe_config);
        })()
        );
    tmk_config::Configuration config = tmk_config::Configuration(new_tuple_type);
    config.Save(configPath);
}