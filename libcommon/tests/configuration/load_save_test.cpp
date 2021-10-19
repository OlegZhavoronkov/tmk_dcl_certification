#include <gtest/gtest.h>
#include <libcommon/configurator.h>
#include <libcommon/config_exceptions.h>
#include <libcommon/ServerSettings.h>
#include <libcommon/FakeCameraConfig.h>
#include <libcommon/PipelineConfig.h>
#include <fstream>
#include <libcommon/configuration_c.h>

namespace tmk_config = tmk::common::configuration;

TEST(DedicatedConfigTest,loadsave)
{
    std::string filename = TEMP_CONFIG_FILE;
    EXPECT_THROW(
        {
            tmk_config::Configuration::Load(std::string());
            tmk_config::Configuration::Instance().Save();
        },
        tmk_config::config_not_found_exception
    );
    EXPECT_THROW(
        {
            {
                std::ofstream stream(filename,std::ios::trunc);
                stream << "string full of garbage<>sss"<<std::endl;
                stream.flush();                    
            }
            tmk_config::Configuration::Load(filename);
        },
        tmk_config::bad_config_exception
    );
    tmk_config::ServerSettings copy;
    EXPECT_NO_THROW(
        {
            auto new_tuple_type=std::make_tuple(tmk_config::ServerSettings(),
                                                tmk_config::FakeCameraConfig(),
                                                ([]()->tmk_config::PipelineConfig
                                                {
                                                    tmk_config::PipelineConfig pipe_config;
                                                    pipe_config.PipelineConfigName()="default";
                                                    return std::move(pipe_config);
                                                })()
                                                );
            tmk_config::Configuration config = tmk_config::Configuration(new_tuple_type);
            config.Save(filename);
            tmk_config::Configuration::Load(filename);        
            copy = static_cast<const tmk_config::ServerSettings&>(tmk_config::Configuration::Instance().getByTag(tmk_config::ServerSettings::configType));                                
        }
    );
    EXPECT_NO_THROW(
        {
            tmk_config::Configuration::Reset();
            const server_settings* p_c_settings= ::loadServerSettings(filename.c_str());
            EXPECT_TRUE(p_c_settings != NULL);
            EXPECT_TRUE(copy.getAddress()==std::string(p_c_settings->address));               
            EXPECT_TRUE(copy.getShmemDataName()==std::string(p_c_settings->shmem_data_name));       
            EXPECT_TRUE(copy.getBoostSegmentName()==std::string(p_c_settings->boost_segment_name_env));
            EXPECT_TRUE(copy.getPort()==p_c_settings->port);      
            EXPECT_TRUE(copy.getCommandQueueSize()==p_c_settings->command_queue_size);            
            EXPECT_TRUE(copy.getShmemDataSize()==p_c_settings->shmem_data_size_env);        
            EXPECT_TRUE(copy.getMinimalBoostSize()==p_c_settings->minimal_boost_size);         
        }
    );
};