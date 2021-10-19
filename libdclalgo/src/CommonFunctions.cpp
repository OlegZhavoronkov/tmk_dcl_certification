//------------------------------------------------------------------------------
//  Created     : 06.04.2021
//  Author      : Golovchanskaya Julia
//  Description : Common functions for different needs (process a string,
//  convert data types, and so on)
//------------------------------------------------------------------------------
#include "CommonFunctions.h"

#include <glog/logging.h>

#include <sstream>
#include <numeric>

std::vector<std::string> split(const std::string &s, char delimiter) {
  std::vector<std::string> elements;
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delimiter)) {
    elements.push_back(item);
  }
  return elements;
}

unsigned
getUintFromString(const std::string &value, unsigned int defaultValue) {
  if (value.empty()) {
    return defaultValue;
  }

  char *endPtr;
  unsigned int result = strtoul(value.c_str(), &endPtr, 0);

  if (*endPtr) {
    return defaultValue;
  }

  return result;
}

double getDoubleFromString(const std::string &value, double defaultValue) {
  if (value.empty()) {
    return defaultValue;
  }

  char *endPtr;
  double result = strtod(value.c_str(), &endPtr);

  if (*endPtr) {
    return defaultValue;
  }

  return result;
}

double getConditionNumber(const cv::Mat &matrix, cv::Mat &inverseMatrix) {
  return 1.0 / cv::invert(matrix, inverseMatrix, cv::DECOMP_SVD);
}

double getMedian(std::vector<double> vec) {
  int vectorLength = vec.size();
  nth_element(vec.begin(), vec.begin() + vectorLength / 2, vec.end());
  if (vectorLength % 2 == 0) {
    nth_element(vec.begin(), vec.begin() + (vectorLength - 1) / 2, vec.end());
    return (vec[(vectorLength - 1) / 2] + vec[vectorLength / 2]) / 2.0;
  } else {
    return vec[vectorLength / 2];
  }
}

double getStd(std::vector<double> vec) {
  double sum = 0.0;
  double mean = 0.0;
  double variance = 0.0;

  sum = std::accumulate(vec.begin(), vec.end(), 0);
  mean = sum / vec.size();

  for (size_t i = 0; i < vec.size(); ++i) {
    variance += pow(vec[i] - mean, 2);
  }
  variance = variance / vec.size();
  return sqrt(variance);
}