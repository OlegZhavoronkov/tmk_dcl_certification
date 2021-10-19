//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

//
// Created by maria on 18.10.2019.
#include "configureCameras.h"

bool stob(const std::string &string) {
    if (string == "true")
        return true;
    if (string == "false")
        return false;
    throw std::logic_error("Can't convert " + string + " to bool");
}

namespace Spinnaker::GenICam {
    void setIntProperty(std::string &name, int value, Spinnaker::CameraPtr &pCam) {
        Spinnaker::GenApi::CIntegerPtr ptrInt = pCam->GetNodeMap().GetNode(name.c_str());
        ptrInt->SetValue(value);
    }
}


void setBoolProperty(std::string &name, bool value, Spinnaker::CameraPtr &pCam) {
    Spinnaker::GenApi::CBooleanPtr ptrBool = pCam->GetNodeMap().GetNode(name.c_str());
    ptrBool->SetValue(value);
}


void setFloatProperty(std::string &name, float value, Spinnaker::CameraPtr &pCam) {
    Spinnaker::GenApi::CFloatPtr ptrFloat = pCam->GetNodeMap().GetNode(name.c_str());
    ptrFloat->SetValue(value);
}


void setEnumerationProperty(std::string &name, std::string &value, Spinnaker::CameraPtr &pCam) {
    Spinnaker::GenApi::CEnumerationPtr ptrEnumNode = pCam->GetNodeMap().GetNode(name.c_str());
    Spinnaker::GenApi::CEnumEntryPtr ptrEnum = ptrEnumNode->GetEntryByName(value.c_str());
    ptrEnumNode->SetIntValue(ptrEnum->GetValue());
}

bool configureCameras(const std::vector<CameraConfig> &camParamList, const Spinnaker::CameraList& camList, std::vector<Spinnaker::CameraPtr> &pCams) {
    pCams.clear();

    for (auto cam : camParamList) {

        if (cam.sdk == "spin") {
            auto pCam = camList.GetBySerial(std::to_string(cam.serial));

            pCam->Init();
            pCams.emplace_back(pCam);

            for (auto prop: cam.properties) {

                if (prop.type == "enum")
                    setEnumerationProperty(prop.name, prop.value, pCam);

                if (prop.type == "float")
                    setFloatProperty(prop.name, stof(prop.value), pCam);

                if (prop.type == "bool")
                    setBoolProperty(prop.name, stob(prop.value), pCam);

                if (prop.type == "int")
                    Spinnaker::GenICam::setIntProperty(prop.name, std::stoi(prop.value), pCam);
            }
        }
    }
    return pCams.size() == camParamList.size();
}
