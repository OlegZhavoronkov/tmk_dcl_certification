#include "DefectMiddle.h"
#include "defectdetection.h"
#include "dclprocessor/dclprocessor.h"
#include <storage/ObjectDescriptor.h>

#include <glog/logging.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

using namespace tmk::storage;

void DefectModel(ObjectDescriptor &obj, char *boost_path, char *obj_name) {
    LOG(INFO) << "[defect detect]: Hello, object " << obj_name
              << " from " << boost_path
              << "! Detecting defects now..." << std::endl;
    DefectDetectionNetworkV2 model;
    model.load(DefectDetectionMiddleConstants::TorchScriptModulePath);
    auto framesets = obj.framesetsIDs();
    for(auto &framesetID : framesets) {
        auto fs = obj.frameset(framesetID);
        for(auto &frameID : fs().framesIDs()) {
            auto frame = fs().frame(frameID);
            auto img = frame().image(); // cv::Mat
            cv::imshow("image", img);
            cv::waitKey(0);
            auto mask = model.getDefectMask(img);
            LOG(INFO) << "[defect detect]: Got mask, containing "
                      << countNonZero(mask) << " non-zero values" << std::endl;
        }
    }
}
