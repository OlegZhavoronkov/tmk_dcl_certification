//------------------------------------------------------------------------------
//  Created     : 25.06.2021
//  Author      : Alin42
//  Description : A tool that creates "Azimuth map", "Zenith map" and
//                "Map of DoP's" with different expanders from set of images.
//------------------------------------------------------------------------------
#include "PolarExpand.h"
#include "AzimuthEvaluation.h"
#include "ZenithEvaluation.h"
#include "LibraryParameters.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace MapsVariables {
const std::vector<double> polarAngles = {0, 45, 90, 135};
constexpr float zenithN = 1.3;
constexpr int zenithPointsNumber = 2000;
bool ONLYEXPANDDEBAYER = 0;
bool ONLYEXPANDLITE = 0;
bool EXPAND = 1;
bool ZENITH = 1;
bool AZIMUTH = 1;
bool POLARDEGREE = 1;
} // MapsVariables

void saveResult(const std::string &where, const cv::Mat &result) {
    std::ofstream outFile(where + ".txt", std::ios_base::out);
    for (int r = 0; r < result.rows; r++) {
        for (int c = 0; c < result.cols; c++) {
            outFile << result.at<double>(r,c);
            if (c + 1 != result.cols) {
                outFile << ","; // csv format
            }
        } 
        outFile << std::endl;
    } 
}

void handleExpandedImage(const std::vector<cv::Mat> &expanded,
                         const std::string &imgName, const std::string &path) {
    if (MapsVariables::EXPAND) {
        cv::imwrite(path + "/expanded/" + imgName + "_0" + ".png", expanded[0]);
        cv::imwrite(path + "/expanded/" + imgName + "_1" + ".png", expanded[1]);
        cv::imwrite(path + "/expanded/" + imgName + "_2" + ".png", expanded[2]);
        cv::imwrite(path + "/expanded/" + imgName + "_3" + ".png", expanded[3]);
    }
    cv::Mat result;
    cv::Mat zenithResult;
    if (MapsVariables::AZIMUTH) {
        computeAzimuth(expanded, result);
        saveResult(path + "/azimuth/" + imgName, result);
    }
    if (MapsVariables::POLARDEGREE || MapsVariables::ZENITH) {
        computePolDegree(expanded, result);
    }
    if (MapsVariables::POLARDEGREE) {
        saveResult(path + "/polar_degree/" + imgName, result);
    }
    if (MapsVariables::ZENITH) {
        zenithResult = zenith_eval(ReflectionDiffuse, result,
                             MapsVariables::zenithN,
                             MapsVariables::zenithPointsNumber);
        saveResult(path + "/zenith/diffuse/" + imgName, result);
        zenithResult = zenith_eval(ReflectionSpecular, result,
                             MapsVariables::zenithN,
                             MapsVariables::zenithPointsNumber);
        saveResult(path + "/zenith/specular/" + imgName, result);
    }
}

template<typename T>
void handleImage(PolarExpand<T> &expander, const cv::Mat &image,
                 const std::string &imgName, const std::string &path) {
    std::vector<cv::Mat> expanded;
    std::cout << "Processing " << imgName << "..." << std::endl;
    if (!MapsVariables::ONLYEXPANDLITE) {
        expander.expandDebayer_fast(image, expanded);
        handleExpandedImage(expanded, imgName, path + "/expandDebayer/");
    }
    if (!MapsVariables::ONLYEXPANDDEBAYER) {
        expander.expandLite(image, expanded);
        handleExpandedImage(expanded, imgName, path + "/expandLite/");
    }
    std::cout << "Done " << imgName << "!" << std::endl;
}

int main(int argc, char* argv[]) {
  LibraryParameters::configure(
      LibraryParameters::PolarizationSensorOrder::order90_45_135_0);
  std::string path;
    if (argc < 2 || argc > 9) {
        std::cout << "Usage: " << argv[0] << " <folder>" << std::endl;
        std::cout << "Expecting it to contain images." << std::endl;
        std::cout << "Also you can use flags:" << std::endl;
        std::cout << "  --no-expand" << std::endl;
        std::cout << "  --no-azimuth" << std::endl;
        std::cout << "  --no-zenith" << std::endl;
        std::cout << "  --no-polar_degree" << std::endl;
        std::cout << "  --only-expandDebayer" << std::endl;
        std::cout << "  --only-expandLite" << std::endl;
        return 1;
    } else {
        // parse options
        path = argv[argc - 1];
        for (int i = 1; i < argc - 1; ++i) {
            if (std::string(argv[i]) == "--no-expand") {
                MapsVariables::EXPAND = 0;
            } else if (std::string(argv[i]) ==  "--no-azimuth") {
                MapsVariables::AZIMUTH = 0;
            } else if (std::string(argv[i]) == "--no-zenith") {
                MapsVariables::ZENITH = 0;
            } else if (std::string(argv[i]) == "--no-polar_degree") {
                MapsVariables::POLARDEGREE = 0;
            } else if (std::string(argv[i]) == "--only-expandDebayer") {
                MapsVariables::ONLYEXPANDDEBAYER = 1;
            } else if (std::string(argv[i]) == "--only-expandLite") {
                MapsVariables::ONLYEXPANDLITE = 1;
            } else {
                std::cout << "Unrecognized option: " << argv[i];
                std::cout << ". Skipped" << std::endl;
            }
        }
    }
    // create needed directories
    if (!MapsVariables::ONLYEXPANDLITE) {
        fs::create_directory(path + "/expandDebayer/");
        if (MapsVariables::EXPAND) {
            fs::create_directory(path + "/expandDebayer/expanded/");
        }
        if (MapsVariables::AZIMUTH) {
            fs::create_directory(path + "/expandDebayer/azimuth/");
        }
        if (MapsVariables::ZENITH) {
            fs::create_directory(path + "/expandDebayer/zenith/");
            fs::create_directory(path + "/expandDebayer/zenith/diffuse/");
            fs::create_directory(path + "/expandDebayer/zenith/specular/");
        }
        if (MapsVariables::POLARDEGREE) {
            fs::create_directory(path + "/expandDebayer/polar_degree/");
        }
    }
    if (!MapsVariables::ONLYEXPANDDEBAYER) {
        fs::create_directory(path + "/expandLite/");
        if (MapsVariables::EXPAND) {
            fs::create_directory(path + "/expandLite/expanded/");
        }
        if (MapsVariables::AZIMUTH) {
            fs::create_directory(path + "/expandLite/azimuth/");
        }
        if (MapsVariables::ZENITH) {
            fs::create_directory(path + "/expandLite/zenith/");
            fs::create_directory(path + "/expandLite/zenith/diffuse/");
            fs::create_directory(path + "/expandLite/zenith/specular/");
        }
        if (MapsVariables::POLARDEGREE) {
            fs::create_directory(path + "/expandLite/polar_degree/");
        }
    }
    for(auto& p: fs::directory_iterator(path)) {
        if (p.is_regular_file()) {
            std::string imgName = p.path().stem();
            cv::Mat image;
            try {
                image = cv::imread(path + "/"+ std::string(p.path().filename()),
                                   cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
            } catch (...) {
                std::cout << "Could not handle " << imgName
                          << ". Is it an image?" << std::endl;
                continue;
            }
            if (image.type() / (1 << CV_CN_SHIFT) != 0) {
                std::cout << "Caution! Processing multichannel image -- "
                          << imgName << std::endl;
            }
            // % (1 << CV_CN_SHIFT) -- for multichannel,
            // see /usr/local/include/opencv4/opencv2/core/hal/interface.h
            if (image.type() % (1 << CV_CN_SHIFT) == CV_8U) {
                PolarExpand<uint8_t> expander;
                handleImage(expander, image, imgName, path);
            } else if (image.type() % (1 << CV_CN_SHIFT) == CV_16U) {
                PolarExpand<uint16_t> expander;
                handleImage(expander, image, imgName, path);
            } else {
                std::cout << "Error: image " << imgName
                          << " is not of a known type." << std::endl;
                std::cout << "It must be mono8/16 "
                          << "(opencv's type CV_8U or CV_16U)" << std::endl;
            }
        }
    }
    return 0;
}
