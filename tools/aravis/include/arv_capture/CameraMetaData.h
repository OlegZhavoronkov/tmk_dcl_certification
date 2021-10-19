//------------------------------------------------------------------------------
//  Created     : 04.05.2021
//  Author      : Victor Kats
//  Description : Functions to manipulate with camera metadata
//------------------------------------------------------------------------------

#ifndef ARV_CAPTURE_CAMERAMETADATA_H
#define ARV_CAPTURE_CAMERAMETADATA_H

#ifdef __cplusplus
#define EXTERNC extern "C"

#include <arv.h>
#include <nlohmann/json.hpp>

#include <string>
#include <vector>

nlohmann::json serialiseCameraFeatures(ArvCamera *camera, std::vector<std::string> features);

std::string cameraFeaturesToCommandString(nlohmann::json &meta);

#else
#define EXTERNC
#endif

EXTERNC char *getCameraCommandString(const char *data);

#undef EXTERNC

#endif //ARV_CAPTURE_CAMERAMETADATA_H
