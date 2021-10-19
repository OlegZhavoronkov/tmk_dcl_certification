#include "savemiddle.h"
#include "dclprocessor/dclprocessor.h"
#include <storage/ObjectDescriptor.h>

#include <glog/logging.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <nlohmann/json.hpp>

#include <fstream>
#include <string>
#include <filesystem>
#include <chrono>
#include <iomanip>
#ifdef __cpp_lib_format
#include <format>
#endif

using namespace tmk::storage;
using json = nlohmann::json;

namespace SaveVariables {
bool config = 0;
int sourceN;
std::string saveFormat;
std::string pathToSave;
std::string saveExtension;
} // SaveVariables

void saveMiddleConfigDefault() {
    LOG(INFO) << "[save block]: Using default parameters" << std::endl;

    SaveVariables::saveFormat = SaveConstants::defaultSaveFormat;
    SaveVariables::pathToSave = SaveConstants::defaultPathToSave;
    SaveVariables::saveExtension = SaveConstants::defaultSaveExtension;
    SaveVariables::sourceN = SaveConstants::defaultSourceN;
    SaveVariables::config = 1;
}

void saveMiddleConfig(const std::string &fileName) {
    if (fileName.empty() || fileName[0] == '\0') {
        LOG(INFO) << "[save block]: Empty string instead of fileName(.json)"
                  << std::endl;
        saveMiddleConfigDefault();
        return;
    }
    std::ifstream jsonFile(fileName);
    if (!jsonFile.is_open() || !jsonFile.good()) {
        LOG(INFO) << "[save block]: Failed to open " << fileName << std::endl;
        saveMiddleConfigDefault();
        return;
    }
    json jsonObj;
    try {
        jsonFile >> jsonObj;
    } catch (std::logic_error &e) {
        LOG(INFO) << "[save block]: Failed parse json file "
                   << fileName << std::endl;
        jsonFile.close();
        saveMiddleConfigDefault();
        return;
    }
    jsonFile.close();
    try {
        SaveVariables::sourceN = jsonObj["sourceN"].get<int>();
        SaveVariables::saveFormat = jsonObj["saveFormat"].get<std::string>();
        SaveVariables::pathToSave = jsonObj["pathToSave"].get<std::string>();
        SaveVariables::saveExtension =
           jsonObj["saveExtension"].get<std::string>();
    } catch (std::exception &e) {
        LOG(INFO) << "[save block]: File " << fileName << " has wrong format. "
                   << "It must contain saveFormat, pathToSave and saveExtension"
                   << std::endl;
        jsonFile.close();
        saveMiddleConfigDefault();
        return;
    }
    SaveVariables::config = 1;
}

// FIXME: implemented basic formatting, if used not on c++23. Not all features
std::string saveFormat(tmk::storage::FrameID &frameID, std::time_t saveTime) {
#ifdef __cpp_lib_format
    return std::format(SaveVariables::saveFormat, frameID.name, saveTime);
#else
    std::ostringstream ss;
    auto bra = SaveVariables::saveFormat.find('{');
    auto ket = SaveVariables::saveFormat.find('}', bra);
    size_t lastPosition = 0;
    int brackets = 0;
    while (bra != std::string::npos) {
        ss << SaveVariables::saveFormat.substr(lastPosition,bra - lastPosition);
        if (brackets == 0) {
            ss << frameID.name;
            ++brackets;
        } else if (brackets == 1) {
            ss << std::put_time(std::localtime(&saveTime), "%T");
            ++brackets;
        } else {
            LOG(INFO) << "[save block]: Something is wrong with format! "
                      << "Using as default: {}_{%a %b %e %H:%M:%S %Y}"
                      << std::endl;
            return std::string(frameID.name) + '_' +
                   std::asctime(std::localtime(&saveTime));
        }
        lastPosition = ket + 1;
        bra = SaveVariables::saveFormat.find('{', ket);
        ket = SaveVariables::saveFormat.find('}', bra);
    }
    ss << SaveVariables::saveFormat.substr(lastPosition);
    return ss.str();
#endif
}

// extern "C"
void SaveBlock(ObjectDescriptor &obj, char *boost_path, char *obj_name) {
    if (SaveVariables::config == 0) {
        saveMiddleConfig(SaveConstants::configPath);
    }
    LOG(INFO) << "[save block]: Hello, object " << obj_name
              << " from " << boost_path
              << "! Saving now..." << std::endl;
    auto framesets = obj.framesetsIDs();
    for (auto &framesetID : framesets) {
        LOG(INFO) << "[save block]: Got frameset "
                  << framesetID << ", saving..." << std::endl;
        std::string dirToSave = SaveVariables::pathToSave + "/" +
                                framesetID.name;
        std::filesystem::create_directories(dirToSave);

        auto fs = obj.frameset(framesetID);
        for (auto &frameID : fs().framesIDs()) {
            auto frame = fs().frame(frameID);
            if (frame().source == SaveVariables::sourceN) {
                std::string fullPathToSave = dirToSave + "/" + saveFormat(
                    frameID,
                    std::chrono::system_clock::to_time_t(frame().timeStamp))
                    + "." + SaveVariables::saveExtension;
                cv::Mat img = frame().image();
                if (!cv::imwrite(fullPathToSave, img)) {
                    LOG(INFO) << "[save block]: Could not save frame "
                              << frameID << std::endl;
                }
            }
        }
    }
}
