//------------------------------------------------------------------------------
//  Created     : 12.04.2021
//  Author      : Golovchanskaya Julia
//  Description : Realization of "Edge Aware" algorithm.
//------------------------------------------------------------------------------

#ifndef EDGE_AWARE_H
#define EDGE_AWARE_H

#include "dclalgo_export.h"

#include <opencv2/core.hpp>

DCLALGO_EXPORT cv::Mat getFilteredCannyImage(const cv::Mat &image);
DCLALGO_EXPORT cv::Mat
getExtendedEdges(const cv::Mat &e, int stages, int stage = 1);
DCLALGO_EXPORT std::vector<cv::Mat>
getPreparedEdges(const std::vector<cv::Mat> &expandedSourcesEasy);

DCLALGO_EXPORT double
CovEstimate(const cv::Mat &Im, int i, int j, bool rhombus = false);
DCLALGO_EXPORT cv::Mat PixCovariance1(const cv::Mat &Im, int i, int j, int M);
DCLALGO_EXPORT cv::Mat PixCovariance2(const cv::Mat &Im, int i, int j, int M);
DCLALGO_EXPORT double BlEstimate1(const cv::Mat &Im, int i, int j);
DCLALGO_EXPORT double BlEstimate2(const cv::Mat &Im, int i, int j);

DCLALGO_EXPORT std::vector<cv::Mat>
firstStageEdgesEstimation(const cv::Mat &originalImage,
                          const std::vector<cv::Mat> &edges,
                          const std::vector<cv::Mat> &expandedSource);
DCLALGO_EXPORT void
secondStageEdgesEstimation(const std::vector<cv::Mat> &edges,
                           std::vector<cv::Mat> &firstStageImages);
#endif //EDGE_AWARE_H
