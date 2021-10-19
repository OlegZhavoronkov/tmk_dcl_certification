#include <gtest/gtest.h>
#include "libcommon/configurator.h"
//#include <config_exceptions.h>
#include "libcommon/ServerSettings.h"
#include "libcommon/FakeCameraConfig.h"
#include "libcommon/PipelineConfig.h"
//#include <fstream>
#include <libcommon/configuration_c.h>

namespace tmk_config = tmk::common::configuration;

TEST(ConfigurationTest,c_interface)
{
    std::string filename = std::string("c_interface_test_")+TEMP_CONFIG_FILE;
    tmk_config::ServerSettings server_settings_example( "192.168.2.3",
                                                        123,
                                                        30,
                                                        "NEW_SHMEM_NAME",
                                                        200000,
                                                        "NEW_BOOST_SEGEMNT_NAME",
                                                        123456789);
    tmk_config::Configuration::Reset();
    const server_settings* pSettings = NULL;
    EXPECT_NO_THROW(
        EXPECT_TRUE(::loadServerSettings(NULL) == NULL);
        EXPECT_TRUE(::getServerSettings() == NULL);
    );
    
    {
        auto new_tuple_type=std::make_tuple(server_settings_example,
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
    }
    EXPECT_NO_THROW(
        pSettings = ::loadServerSettings(filename.c_str());
        EXPECT_TRUE(pSettings != NULL);
        EXPECT_TRUE(server_settings_example.getAddress()==          std::string(pSettings->address));               
        EXPECT_TRUE(server_settings_example.getShmemDataName()==    std::string(pSettings->shmem_data_name));       
        EXPECT_TRUE(server_settings_example.getBoostSegmentName()== std::string(pSettings->boost_segment_name_env));
        EXPECT_TRUE(server_settings_example.getPort()==             pSettings->port);      
        EXPECT_TRUE(server_settings_example.getCommandQueueSize()== pSettings->command_queue_size);            
        EXPECT_TRUE(server_settings_example.getShmemDataSize()==    pSettings->shmem_data_size_env);        
        EXPECT_TRUE(server_settings_example.getMinimalBoostSize()== pSettings->minimal_boost_size);   
        const server_settings* pSettings2 = ::getServerSettings();
        EXPECT_EQ(pSettings,pSettings2);
        EXPECT_TRUE(server_settings_example.getAddress()==          std::string(pSettings2->address));               
        EXPECT_TRUE(server_settings_example.getShmemDataName()==    std::string(pSettings2->shmem_data_name));       
        EXPECT_TRUE(server_settings_example.getBoostSegmentName()== std::string(pSettings2->boost_segment_name_env));
        EXPECT_TRUE(server_settings_example.getPort()==             pSettings2->port);      
        EXPECT_TRUE(server_settings_example.getCommandQueueSize()== pSettings2->command_queue_size);            
        EXPECT_TRUE(server_settings_example.getShmemDataSize()==    pSettings2->shmem_data_size_env);        
        EXPECT_TRUE(server_settings_example.getMinimalBoostSize()== pSettings2->minimal_boost_size);   
    )  ; 
}