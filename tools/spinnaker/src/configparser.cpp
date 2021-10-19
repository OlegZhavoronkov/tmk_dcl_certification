//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

#include <string>
#include "configparser.h"

std::vector<CameraConfig> ConfigParser::getCamerasList(std::string filename)
{
    JsonLoader loader(filename);

    std::vector<Property> defaults;
    loader.visit(defaults, "DefaultProperties");

    std::vector<CameraConfig> configs;
    loader.visit(configs, "CamerasList");

    for (int i = 0; i < configs.size(); i++)
        configs[i].properties.insert(configs[i].properties.begin(), defaults.begin(), defaults.end());

    return configs;
}

std::vector<TriggerConfig> ConfigParser::getTriggerList(std::string filename)
{
    std::vector<TriggerConfig> triggerList;
    JsonLoader loader(filename);
    loader.visit(triggerList, "TriggerList");
    return triggerList;
}
