#include <libcommon/meta_info.h>
#include <libcommon/ServerSettings.h>
#include <list>
#include <nlohmann/json.hpp>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <libcommon/config_factory.h>
#include <libcommon/configurator.h>
#include <libcommon/config_exceptions.h>
#include <libcommon/PipelineConfig.h>
#include <libcommon/FakeCameraConfig.h>
#include <boost/format.hpp>
#include <filesystem>
namespace tmk_config = tmk::common::configuration;
void testCreate(const std::string& filename)
{
    tmk_config::meta_info info,info1,info2;
    nlohmann::json json;
    int i=0;
    for(auto infos : {info,info1,info2})
    {
        std::stringstream str;
        str<<"infos_"<<i++;
        tmk_config::_fabrics.ToJson(json,infos,str.str());
        infos.save(json);
    }
    tmk_config::ServerSettings serverSetts;
    serverSetts.save(json);
    for(auto tt : { std::make_tuple(2,"config_demo"),
                    std::make_tuple(5,"config_default") })
    {
        tmk_config::PipelineConfig pipelineConfig;    
        for(int i=0; i< std::get<0>(tt); i++)
        {
            tmk_config::PipelineNodeConfig node(    i,
                            (boost::format("bin//block_%d") 
                                % (i+1)).str(),
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
                                    (i < (std::get<0>(tt)-1) )? 
                                        tmk_config::PipelineNodeType::MIDDLE : 
                                        tmk_config::PipelineNodeType::LAST,
                            i < (std::get<0>(tt)-1) ? i+1 : -1,
                            (boost::format("node_%d")%i).str() );
            pipelineConfig.AddNode(node);                                                        
        }
        pipelineConfig.PipelineConfigName() = std::get<1>(tt);
        pipelineConfig.save(json);
    }
    
    tmk_config::FakeCameraConfig fake_camera_config;
    fake_camera_config.save( json );
    
    {
        {
            std::ofstream out(  filename,
                                std::ios_base::openmode::_S_out 
                              | std::ios_base::openmode::_S_trunc );
            out << std::setw(4)<<json << std::endl;
            out.close();
        }
    }
}

void testRead(const std::string& filename)
{
    try
    {
        tmk_config::Configuration::Load(filename);
        std::filesystem::path oldPath(filename);
        auto oldfilename = oldPath.filename().stem();
        oldfilename+= std::string(".modified");
        tmk_config::Configuration::Instance()
                                    .Save(oldPath
                                    .replace_filename(oldfilename )
                                    .string());
    }
    catch(tmk_config::configuration_exception& ex)
    {
        std::cout   << "exception : "  << std::endl
                    << ex.what()  << std::endl
                    ;
    }
    
}

int main(int argc,char** argv)
{
    std::string filename = "server_settings_multiple_metas_pipeline.json";
    try
    {
        testCreate(filename);
        testRead(filename);
    }

    catch(const nlohmann::json::type_error& terr)
    {
        std::cout   << "exception : "  << std::endl
                    << terr.what()  << std::endl
                    ;
    }
    catch(const std::exception& ex)
    {
        std::cout   << "exception : "  << std::endl
                    << ex.what()  << std::endl
                    ;
    }

    
}
