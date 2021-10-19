#include <gtest/gtest.h>
#include <libcommon/meta_info.h>
#include <libcommon/PipelineConfig.h>
#include <libcommon/ServerSettings.h>
#include <libcommon/server_settings_c.h>
#include <nlohmann/json.hpp>
#include <boost/format.hpp>

namespace tmk_config = tmk::common::configuration;



TEST(DedicatedConfigTest,configs)
{
    ASSERT_NO_THROW(
        {
            tmk_config::meta_info meta_info1;
            nlohmann::json json= meta_info1.toJson();
            tmk_config::meta_info meta_info2 = tmk_config::meta_info::create(json);
            ASSERT_TRUE(meta_info1 ==meta_info2);
            meta_info2.touch();
            ASSERT_FALSE (meta_info1==meta_info2);
        }
        {
            for(auto tt : { std::make_tuple(1,"config_single"),
                            std::make_tuple(2,"config_demo"),
                            std::make_tuple(5,"config_default"),
                            })
            {
                tmk_config::PipelineConfig pipelineConfig;    
                for(int i=0; i< std::get<0>(tt); i++)
                {
                    tmk_config::PipelineNodeConfig node(    i,
                                                            (boost::format("bin//block_%d") % (i+1)).str(),
                                                            (
                                                                [=]()->auto
                                                                {
                                                                    std::list<std::string> pars;
                                                                    int n=0;
                                                                    std::generate_n(std::back_inserter(pars),
                                                                                    i+1,
                                                                                    [&]()->auto
                                                                                    {
                                                                                        return (boost::format("par_%d")% n++).str();
                                                                                    });
                                                                    return std::move(pars);                                                                        
                                                                }
                                                            )(),
                                                            i == 0 ? tmk_config::PipelineNodeType::FIRST :
                                                                    (i < (std::get<0>(tt)-1) )? tmk_config::PipelineNodeType::MIDDLE : tmk_config::PipelineNodeType::LAST,
                                                            i < (std::get<0>(tt)-1) ? i+1 : -1,
                                                            (boost::format("node_%d")%i).str()
                                                            );
                    pipelineConfig.AddNode(node);                                                        
                }
                nlohmann::json json;
                pipelineConfig.PipelineConfigName() = std::get<1>(tt);
                json = pipelineConfig.toJson();
                tmk_config::PipelineConfig plc2 = tmk_config::PipelineConfig(json);
                ASSERT_FALSE(pipelineConfig == plc2) ;
                plc2.PipelineConfigName() = pipelineConfig.PipelineConfigName();
                ASSERT_TRUE(pipelineConfig==plc2) ;
            }

        }
        {
            tmk_config::ServerSettings server_setts;
            auto json = server_setts.toJson();
            tmk_config::ServerSettings server_settings2(json);
            const server_settings* pRaw= static_cast<const server_settings*>(&server_setts);
            ASSERT_TRUE(server_settings2 == server_setts);
            ASSERT_TRUE (   std::string(pRaw ->address) == server_setts.getAddress() );
            ASSERT_TRUE (    pRaw ->port == server_setts.getPort() );
            ASSERT_TRUE (    pRaw ->command_queue_size == server_setts.getCommandQueueSize() );
            ASSERT_TRUE (    std::string(pRaw ->shmem_data_name) == server_setts.getShmemDataName());
            ASSERT_TRUE (    pRaw ->shmem_data_size_env == server_setts.getShmemDataSize() ); 
            ASSERT_TRUE (    std::string(pRaw ->boost_segment_name_env) == server_setts.getBoostSegmentName() );
            ASSERT_TRUE (    pRaw ->minimal_boost_size == server_setts.getMinimalBoostSize());
        }
    );   
}