//------------------------------------------------------------------------------
//  Created     : 16.06.2021
//  Author      : Alin42
//  Description : A tool that demonstrates "expandLite" function.
//                It takes an image and splits it into 4 by their pol. angle
//                Unlike "expandEasy" tries to remove diff. of pixel position
//------------------------------------------------------------------------------
#include "PolarExpand.h"

#include <opencv2/highgui.hpp>
#include <iostream>

namespace ExpanderConstants {
  const std::string defaultPathToOriginalImage = "../data/def1_SPN0.bmp";
  const std::string defaultPathToSave = "../tools/";
  const std::string name_1 = "expandLite_1.tiff";
  const std::string name_2 = "expandLite_2.tiff";
  const std::string name_3 = "expandLite_3.tiff";
  const std::string name_4 = "expandLite_4.tiff";
} // ExpanderConstants

int main(int argc, char* argv[]) {
    std::string pathToOriginalImage;
    std::string pathToSave[4];
    if (argc == 1) {
        std::cout << "Using default arguments:" << std::endl;
        std::cout << " Image path: "
                  << ExpanderConstants::defaultPathToOriginalImage << std::endl;
        std::cout << " Path where images will be stored: "
                  << ExpanderConstants::defaultPathToSave << std::endl;
        std::cout << "To change it use "
                  << argv[0] << " <image> <where_to_save>" << std::endl;
        pathToOriginalImage = ExpanderConstants::defaultPathToOriginalImage;
        pathToSave[0] = ExpanderConstants::defaultPathToSave +
                        ExpanderConstants::name_1;
        pathToSave[1] = ExpanderConstants::defaultPathToSave +
                        ExpanderConstants::name_2;
        pathToSave[2] = ExpanderConstants::defaultPathToSave +
                        ExpanderConstants::name_3;
        pathToSave[3] = ExpanderConstants::defaultPathToSave +
                        ExpanderConstants::name_4;
    } else if (argc != 3) {
        std::cout << "Didn't get that. Please use " << argv[0] << " or "
                  << argv[0] << " <image> <where_to_save>" << std::endl;
        return 1;
    } else {
        std::cout << "Using these arguments:" << std::endl;
        std::cout << " Image path: " << argv[1] << std::endl;
        std::cout << " Path where images will be stored: "
                  << argv[2] << std::endl;
        pathToOriginalImage = argv[1];
        pathToSave[0] = argv[2] + ExpanderConstants::name_1;
        pathToSave[1] = argv[2] + ExpanderConstants::name_2;
        pathToSave[2] = argv[2] + ExpanderConstants::name_3;
        pathToSave[3] = argv[2] + ExpanderConstants::name_4;
    }
    cv::Mat image = cv::imread(pathToOriginalImage,
                               cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
    std::vector<cv::Mat> result;

    if (image.type() == CV_8UC1) {
      PolarExpand<uint8_t> expander;
      expander.expandLite(image, result);
    } else if (image.type() == CV_16UC1) {
      PolarExpand<uint16_t> expander;
      expander.expandLite(image, result);
    } else {
      std::cout << "Error: image is not of a known type." << std::endl;
      std::cout << "It must be mono8/16 (opencv's type CV_8UC1 or CV_16UC1).";
      std::cout << std::endl;
      return 1;
    }

    for (int i = 0; i < 4; ++i)
        cv::imwrite(pathToSave[i], result[i]);
    return 0;
}
