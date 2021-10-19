#ifndef POLAR_DECOMPOSITION_H
#define POLAR_DECOMPOSITION_H

#include "dclalgo_export.h"

#include <vector>
#include <opencv2/core/mat.hpp>

DCLALGO_EXPORT cv::Mat
pixelModel(const cv::Mat &I_max, const cv::Mat &I_min, const cv::Mat &phi,
           const cv::Mat &phi_pol);

DCLALGO_EXPORT cv::Mat
diffuseModel(double n, const cv::Mat &theta);

DCLALGO_EXPORT cv::Mat
specularModel(double n, const cv::Mat &theta);

DCLALGO_EXPORT cv::Mat
iMin(const cv::Mat &iMax, const cv::Mat rho);

DCLALGO_EXPORT std::vector<cv::Mat>
decomposeImages(const cv::Mat iMax, const cv::Mat &phi, const cv::Mat &rho,
                const std::vector<double> angles);

#endif //POLAR_DECOMPOSITION_H
