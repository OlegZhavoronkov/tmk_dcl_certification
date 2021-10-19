//-----------------------------------------------------------------------
//  Created     : 18.05.21
//  Author      : Alin42
//  Description : Middle block, which takes existing defect detector and
//                does something with it and object
//-----------------------------------------------------------------------

#ifndef _DEFECT_MIDDLE_H_
#define _DEFECT_MIDDLE_H_

#include "defectdetection.h"
#include "dclprocessor/dclprocessor.h"
#include <storage/ObjectDescriptor.h>

#include <glog/logging.h>

#include <opencv2/core.hpp>

using namespace tmk::storage;

/* Path to learned data model */
namespace DefectDetectionMiddleConstants {
const std::string TorchScriptModulePath = "../dclcomponents/defectmiddle/data/";
} // namespace DefectDetectionMiddleConstants
/* or */
/*
#include <nlohmann/json.hpp>
#include <fstream>
using json = nlohmann::json;
inline std::string defect_config(const std::string fileName) {
    if (fileName.empty() || fileName[0] == '\0') {
        LOG(ERROR) << "[defect detect]: Empty string instead of fileName(.json)"
                   << std::endl;
        return "";
    }
    std::ifstream jsonFile(fileName);
    if (!jsonFile.is_open() || !jsonFile.good()) {
        LOG(ERROR) << "[defect detect]: Failed to open "
                   << fileName << std::endl;
        return "";
    }
    json jsonObj;
    try {
        jsonFile >> jsonObj;
    } catch (std::logic_error &e) {
        LOG(ERROR) << "[defect detect]: Failed parse json file "
                   << fileName << std::endl;
        jsonFile.close();
        return "";
    }
    jsonFile.close();
    //json::iterator it = jsonObj.begin();
    return jsonObj["path_to_trained_model"].get<std::string>();
}
DefectDetectionMiddleConstants::TorchScriptModulePath = defect_config(
    "../dclcomponents/defectmiddle/data/defectmiddle.json");
*/

void DefectModel(ObjectDescriptor &, char *boost_path, char *obj_name);

#endif // _DEFECT_MIDDLE_H_
