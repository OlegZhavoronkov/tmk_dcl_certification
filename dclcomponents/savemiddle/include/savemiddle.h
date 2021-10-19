//-----------------------------------------------------------------------
//  Created     : 10.06.21
//  Author      : Alin42
//  Description : Middle block, which saves all data
//                from source #SourceN to the computer
//-----------------------------------------------------------------------

#ifndef SAVE_MIDDLE_H
#define SAVE_MIDDLE_H

#include "dclprocessor/dclprocessor.h"
#include <storage/ObjectDescriptor.h>

#include <glog/logging.h>
#include <opencv2/core.hpp>

#include <string>

namespace SaveConstants {
const std::string configPath =
    "../data/savemiddle/savemiddle.json";
// if used in < c++23 standard
// will be used in as in simplified standard library header <format>
// something like -- std::format(saveFormat, <name>, <time>);
// But if on c++23 -- really in std::format(saveFormat, <name>, <time>);
const std::string defaultSaveFormat = "{}_{}";
const std::string defaultPathToSave = "../data/savemiddle/";
const std::string defaultSaveExtension = "tiff";
constexpr int defaultSourceN = 1;
} // SaveConstants

void saveMiddleConfigDefault();
void saveMiddleConfig(const std::string &fileName);

// extern "C"
void SaveBlock(ObjectDescriptor &, char *boost_path, char *obj_name);

#endif // SAVE_MIDDLE_H
