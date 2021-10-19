//------------------------------------------------------------------------------
//  Created     : 06.04.2021
//  Author      : Golovchanskaya Julia
//  Description : Common functions for different needs (process a string,
//  convert data types, and so on)
//------------------------------------------------------------------------------
#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include <vector>
#include <string>
#include <glob.h>

#include <opencv2/core.hpp>

inline std::vector<std::string>
getFilePaths(const std::string &filePathsPattern) {
  glob_t globResult;
  glob(filePathsPattern.c_str(), GLOB_TILDE, NULL, &globResult);
  std::vector<std::string> filePaths;
  for (size_t i = 0; i < globResult.gl_pathc; ++i) {
    filePaths.emplace_back(std::string(globResult.gl_pathv[i]));
  }
  globfree(&globResult);
  return filePaths;
}

std::vector<std::string> split(const std::string &s, char delimiter);

unsigned getUintFromString(const std::string &value, unsigned int defaultValue);
double getDoubleFromString(const std::string &value, double defaultValue);

double getConditionNumber(const cv::Mat &matrix, cv::Mat &inverseMatrix);

double getMedian(std::vector<double> vec);
double getStd(std::vector<double> vec);

inline std::vector<cv::Point3f>
createChessboardPoints(const int boardHeight, const int boardWidth,
                       const int squareSize) {
  std::vector<cv::Point3f> objectPoints;
  for (int i = 0; i < boardHeight; i++) {
    for (int j = 0; j < boardWidth; j++) {
      objectPoints.emplace_back(
      cv::Point3f((float) j * squareSize, (float) i * squareSize,
                  0));
    }
  }
  return objectPoints;
}

class WelfordOnlineAlgorithm {
public:
  void clean() {
    numberOfSamples = mean0 = std0 = stDeviation = mean = 0;
  }

  float numberOfSamples = 0;
  float mean0 = 0;
  float std0 = 0;

  float stDeviation = 0;
  float mean = 0;

  void update(float x) {
    numberOfSamples += 1;
    if (numberOfSamples == 1) {
      mean0 = x;
      std0 = 0;
    } else {
      float Mnext = mean0 + (x - mean0) / numberOfSamples;
      std0 = std0 + (x - mean0) * (x - Mnext);
      mean0 = Mnext;
      mean = mean0;
      stDeviation = std0 / (numberOfSamples - 1);
    }
  }
};


#endif //COMMON_FUNCTIONS_H
