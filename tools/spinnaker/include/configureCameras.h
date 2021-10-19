//------------------------------------------------------------------------------
//  Created     : 06.08.21
//  Author      : Victor Kats
//  Description : Import Spinnnaker-based capturer into project (see TMKDCL-380)
//------------------------------------------------------------------------------

//
// Created by victor on 03.02.2020.
//

#ifndef GTS_IDENTIFICATION_CONFIGURECAMERAS_H
#define GTS_IDENTIFICATION_CONFIGURECAMERAS_H

#include <vector>
#include <Spinnaker.h>

#include "configparser.h"

bool stob(const std::string &string);

void setBoolProperty(std::string &name, bool value, Spinnaker::CameraPtr &pCam);
void setFloatProperty(std::string &name, float value, Spinnaker::CameraPtr &pCam);
void setEnumerationProperty(std::string &name, std::string &value, Spinnaker::CameraPtr &pCam);

namespace Spinnaker::GenICam {
    void setIntProperty(std::string &name, int value, Spinnaker::CameraPtr &pCam);
}


bool configureCameras(const std::vector<CameraConfig> &camParamList, const Spinnaker::CameraList& camList, std::vector<Spinnaker::CameraPtr> &pCams);

#endif //GTS_IDENTIFICATION_CONFIGURECAMERAS_H
