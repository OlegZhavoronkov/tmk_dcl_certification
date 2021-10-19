//------------------------------------------------------------------------------
//  Created     : 12.04.2021
//  Author      : Golovchanskaya Julia
//  Description : Realization of "Edge Aware" algorithm.
//------------------------------------------------------------------------------
#include "EdgeAware.h"
#include "CommonFunctions.h"

#include <glog/logging.h>
#include <opencv2/imgproc.hpp>

namespace EdgeAwareConstants {
  // getFilteredCannyImage
  constexpr int ksize = 3;
  constexpr int edgeThresh = 12;
  constexpr int apertureSize = 3;

  // PixCovariance1, PixCovariance2
  constexpr double conditionNumberThreshold = 1e9;

  // firstStageEdgesEstimation, secondStageEdgesEstimation
  constexpr int imageBorder = 10;
}

cv::Mat getFilteredCannyImage(const cv::Mat &image) {
  cv::Mat result;
  cv::Mat blurred;
  blur(image, blurred,
       cv::Size(EdgeAwareConstants::ksize, EdgeAwareConstants::ksize));
  Canny(blurred, result, EdgeAwareConstants::edgeThresh,
        EdgeAwareConstants::edgeThresh * 3, EdgeAwareConstants::apertureSize);
  return result;
}

cv::Mat getExtendedEdges(const cv::Mat &e, int stages, int stage) {
  cv::Mat edges;
  if (stages == stage) {
    edges = e;
    return edges;
  }

  auto[width, height] = e.size();

  cv::Mat edgesTemporary = cv::Mat::zeros(cv::Size(width, height), CV_8U);

  for (int j = 0; j < height; ++j) {
    for (int k = 0; k < width; ++k) {
      if (e.at<uint8_t>(j, k) != 0) {

        // To avoid array out of bounds
        int j_previous = std::max(0, j - 1);
        int k_previous = std::max(0, k - 1);
        int j_next = std::min(height - 1, j + 1);
        int k_next = std::min(width - 1, k + 1);

        edgesTemporary.at<uint8_t>(j_previous, k) = 255;
        edgesTemporary.at<uint8_t>(j_previous, k_previous) = 255;
        edgesTemporary.at<uint8_t>(j_previous, k_next) = 255;

        edgesTemporary.at<uint8_t>(j_next, k) = 255;
        edgesTemporary.at<uint8_t>(j_next, k_previous) = 255;
        edgesTemporary.at<uint8_t>(j_next, k_next) = 255;

        edgesTemporary.at<uint8_t>(j, k_previous) = 255;

        edgesTemporary.at<uint8_t>(j, k_next) = 255;

        edgesTemporary.at<uint8_t>(j, k) = 255;
      }
    }
  }

  edges = getExtendedEdges(edgesTemporary, stages, stage + 1);
  return edges;
}

std::vector<cv::Mat>
getPreparedEdges(const std::vector<cv::Mat> &expandedSourcesEasy) {
  std::vector<cv::Mat> edges;
  edges.resize(expandedSourcesEasy.size());
  for (size_t i = 0; i < edges.size(); ++i) {
    cv::Mat cannyImage = getFilteredCannyImage(expandedSourcesEasy[i]);
    cv::resize(cannyImage, cannyImage, cv::Size(), 2, 2);
    edges[i] = getExtendedEdges(cannyImage, 2);
  }
  return edges;
}

cv::Mat PixCovariance1(const cv::Mat &Im, int i, int j, int M) {
  if (M % 2 == 1) {
    LOG(ERROR) << "EdgeAware: M must be even!";
  }

  auto[width, height]  = Im.size();

  if (i - M < 0 || i + M + 1 >= height || j - M < 0 || j + M + 1 >= width) {
    LOG(ERROR) << "EdgeAware: Too near to the image boundary!";
  }

  cv::Mat C = cv::Mat::zeros(4, M * M, CV_64F);
  cv::Mat y = cv::Mat::zeros(M * M, 1, CV_64F);
  int i_base = i - M;
  int j_base = j - M;

  for (int i_0 = 0; i_0 < M; ++i_0) {
    for (int j_0 = 0; j_0 < M; ++j_0) {
      int index = i_0 * M + j_0;
      int k = i_0 * 2 + i_base + 1;
      int l = j_0 * 2 + j_base + 1;

      C.at<double>(0, index) = Im.at<double>(k - 2, l - 2);
      C.at<double>(1, index) = Im.at<double>(k + 2, l - 2);
      C.at<double>(2, index) = Im.at<double>(k + 2, l + 2);
      C.at<double>(3, index) = Im.at<double>(k - 2, l + 2);

      y.at<double>(index) = Im.at<double>(k, l);
    }
  }

  cv::Mat S1 = C * C.t();
  cv::Mat S1_inv;
  double cn = getConditionNumber(S1, S1_inv);

  cv::Mat alpha;
  if (cn >= EdgeAwareConstants::conditionNumberThreshold) {
    double d02 = abs(
    Im.at<double>(i + 1, j + 1) - Im.at<double>(i - 1, j - 1));
    double d13 = abs(
    Im.at<double>(i + 1, j - 1) - Im.at<double>(i - 1, j + 1));

    double rat = d02 / d13;
    if (rat < 1.0 / 10.0) {
      alpha = cv::Mat(1, 4, CV_64F, {0.5, 0, 0.5, 0});
    } else if (rat > 10) {
      alpha = cv::Mat(1, 4, CV_64F, {0, 0.5, 0, 0.5});
    } else {
      alpha = cv::Mat(1, 4, CV_64F, {0.25, 0.25, 0.25, 0.25});
    }
  } else {
    alpha = S1_inv * (C * y);
  }
  return alpha;
}

cv::Mat PixCovariance2(const cv::Mat &Im, int i, int j, int M) {
  auto[width, height]  = Im.size();
  if (i - M < 0 || i + M + 1 >= height || j - M < 0 || j + M + 1 >= width) {
    LOG(ERROR) << "EdgeAware: Too near to the image boundary!";
  }

  cv::Mat C = cv::Mat::zeros(4, M * M, CV_64F);
  cv::Mat y = cv::Mat::zeros(M * M, 1, CV_64F);

  for (int m = 0; m < M; ++m) {
    int i_0 = i + m;
    int j_0 = j + 1 - M + m;

    for (int n = 0; n < M; ++n) {
      int index = m * M + n;
      int k = i_0 - n;
      int l = j_0 + n;

      C.at<double>(0, index) = Im.at<double>(k, l - 2);
      C.at<double>(1, index) = Im.at<double>(k + 2, l);
      C.at<double>(2, index) = Im.at<double>(k, l + 2);
      C.at<double>(3, index) = Im.at<double>(k - 2, l);

      y.at<double>(index) = Im.at<double>(k, l);
    }
  }

  cv::Mat S1 = C * C.t();
  cv::Mat S1_inv;
  double cn = getConditionNumber(S1, S1_inv);
  cv::Mat alpha;
  if (cn >= EdgeAwareConstants::conditionNumberThreshold) {
    double dx = abs(Im.at<double>(i, j - 1) - Im.at<double>(i, j + 1));
    double dy = abs(Im.at<double>(i - 1, j) - Im.at<double>(i + 1, j));
    double rat = dx / dy;
    if (rat < 1.0 / 10.0) {
      alpha = cv::Mat(1, 4, CV_64F, {0.5, 0, 0.5, 0});
    } else if (rat > 10) {
      alpha = cv::Mat(1, 4, CV_64F, {0, 0.5, 0, 0.5});
    } else {
      alpha = cv::Mat(1, 4, CV_64F, {0.25, 0.25, 0.25, 0.25});
    }
  } else {
    alpha = S1_inv * (C * y);
  }

  return alpha;
}

double CovEstimate(const cv::Mat &Im, int i, int j, bool rhombus) {
  if (Im.type() != 6) {
    LOG(ERROR) << "EdgeAware: Image pixels have to be double!";
  }

  cv::Mat alpha;
  double pix;
  double pix0;
  if (!rhombus) {
    alpha = PixCovariance1(Im, i, j, 6);
    double d02 = double(abs(
    Im.at<double>(i + 1, j + 1) - Im.at<double>(i - 1, j - 1)));
    double d13 = double(abs(
    Im.at<double>(i + 1, j - 1) - Im.at<double>(i - 1, j + 1)));
    double rat = d02 / d13;

    if (rat < 1.0 / 10.0) {
      pix0 = (Im.at<double>(i + 1, j + 1) + Im.at<double>(i - 1, j - 1)) / 2.0;
    } else if (rat > 10) {
      pix0 = (Im.at<double>(i + 1, j - 1) + Im.at<double>(i - 1, j + 1)) / 2.0;
    } else {
      pix0 = (Im.at<double>(i - 1, j - 1) + Im.at<double>(i + 1, j - 1) +
              Im.at<double>(i + 1, j + 1) + Im.at<double>(i - 1, j + 1)) / 4.0;
    }
    pix = alpha.at<double>(0) * Im.at<double>(i - 1, j - 1) +
          alpha.at<double>(1) * Im.at<double>(i + 1, j - 1) +
          alpha.at<double>(2) * Im.at<double>(i + 1, j + 1) +
          alpha.at<double>(3) * Im.at<double>(i - 1, j + 1);
  } else {
    alpha = PixCovariance2(Im, i, j, 6);
    double dx = abs(Im.at<double>(i, j - 1) - Im.at<double>(i, j + 1));
    double dy = abs(Im.at<double>(i - 1, j) - Im.at<double>(i + 1, j));
    double rat = dx / dy;
    if (rat < 1.0 / 10.0) {
      pix0 = (Im.at<double>(i, j - 1) + Im.at<double>(i, j + 1)) / 2.0;
    } else if (rat > 10) {
      pix0 = (Im.at<double>(i - 1, j) + Im.at<double>(i + 1, j)) / 2.0;
    } else {
      pix0 = (Im.at<double>(i, j - 1) + Im.at<double>(i + 1, j) +
              Im.at<double>(i, j + 1) + Im.at<double>(i - 1, j)) / 4.0;
    }
    pix = alpha.at<double>(0) * Im.at<double>(i, j - 1) +
          alpha.at<double>(1) * Im.at<double>(i + 1, j) +
          alpha.at<double>(2) * Im.at<double>(i, j + 1) +
          alpha.at<double>(3) * Im.at<double>(i - 1, j);
  }

  double s = pix0 / pix;
  if (pix < 0 || pix > 255 || s < 0.9 || 1 / s < 0.9) {
    pix = pix0;
  }

  return pix;
}

double BlEstimate1(const cv::Mat &Im, int i, int j) {
  double e = (Im.at<double>(i - 2, j - 2) + Im.at<double>(i + 2, j - 2) +
              Im.at<double>(i + 2, j + 2) + Im.at<double>(i - 2, j + 2)) / 4.0;
  return e;
}

double BlEstimate2(const cv::Mat &Im, int i, int j) {
  double e = (Im.at<double>(i, j - 1) + Im.at<double>(i + 1, j) +
              Im.at<double>(i, j + 1) + Im.at<double>(i - 1, j)) / 4.0;
  return e;
}

std::vector<cv::Mat> firstStageEdgesEstimation(const cv::Mat &originalImage,
                                               const std::vector<cv::Mat> &edges,
                                               const std::vector<cv::Mat> &expandedSource) {
  auto[width, height] = edges[0].size();

  std::vector<cv::Mat> result;
  result.resize(4);
  for (auto &t : result) {
    t = cv::Mat(cv::Size(width, height), CV_64FC1, cv::Scalar(0.0));
  }

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      int isOddByHeight = i % 2;
      int isOddByWidth = j % 2;
      bool isEstimated = !(i - EdgeAwareConstants::imageBorder < 0 ||
                           i + EdgeAwareConstants::imageBorder + 1 >= height ||
                           j - EdgeAwareConstants::imageBorder < 0 ||
                           j + EdgeAwareConstants::imageBorder + 1 >= width);

      if (!isEstimated) {
        result[0].at<double>(i, j) = double(
        expandedSource[0].at<uint8_t>(i, j));
        result[1].at<double>(i, j) = double(
        expandedSource[1].at<uint8_t>(i, j));
        result[2].at<double>(i, j) = double(
        expandedSource[2].at<uint8_t>(i, j));
        result[3].at<double>(i, j) = double(
        expandedSource[3].at<uint8_t>(i, j));
        continue;
      }

      if (isOddByHeight == 1 && isOddByWidth == 1) { // 0
        result[0].at<double>(i,
                             j) = originalImage.at<double>(i, j);
        if (edges[2].at<uint8_t>(i, j)) {
          result[2].at<double>(i, j) = CovEstimate(originalImage, i, j);
        } else {
          result[2].at<double>(i, j) = BlEstimate1(originalImage, i, j);
        }
      } else if (isOddByHeight == 0 && isOddByWidth == 1) { // 45
        result[1].at<double>(i,
                             j) = originalImage.at<double>(i, j);
        if (edges[3].at<uint8_t>(i, j)) {
          result[3].at<double>(i, j) = CovEstimate(originalImage, i, j);
        } else {
          result[3].at<double>(i, j) = BlEstimate1(originalImage, i, j);
        }
      } else if (isOddByHeight == 0 && isOddByWidth == 0) { // 90
        result[2].at<double>(i,
                             j) = originalImage.at<double>(i, j);
        if (edges[0].at<uint8_t>(i, j)) {
          result[0].at<double>(i, j) = CovEstimate(originalImage, i, j);
        } else {
          result[0].at<double>(i, j) = BlEstimate1(originalImage, i, j);
        }
      } else { // 135
        result[3].at<double>(i,
                             j) = originalImage.at<double>(i, j);
        if (edges[1].at<uint8_t>(i, j)) {
          result[1].at<double>(i, j) = CovEstimate(originalImage, i, j);
        } else {
          result[1].at<double>(i, j) = BlEstimate1(originalImage, i, j);
        }
      }
    }
  }
  return result;
}

void secondStageEdgesEstimation(const std::vector<cv::Mat> &edges,
                                std::vector<cv::Mat> &firstStageImages) {
  auto[width, height] = edges[0].size();

  std::vector<cv::Mat> imagesCopy;
  imagesCopy.resize(firstStageImages.size());
  for (size_t i = 0; i < imagesCopy.size(); ++i) {
    imagesCopy[i] = firstStageImages[i].clone();
  }

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      int isHeightOdd = i % 2;
      int isWidthOdd = j % 2;

      bool isEstimated = !(i - EdgeAwareConstants::imageBorder < 0 ||
                           i + EdgeAwareConstants::imageBorder + 1 >= height ||
                           j - EdgeAwareConstants::imageBorder < 0 ||
                           j + EdgeAwareConstants::imageBorder + 1 >= width);

      if (!isEstimated) {
        continue;
      }

      if (isHeightOdd == 1 && isWidthOdd == 1) { // %0
        if (edges[1].at<uint8_t>(i, j) > 0) {
          firstStageImages[1].at<double>(i, j) = CovEstimate(imagesCopy[1], i,
                                                             j,
                                                             true);
        } else {
          firstStageImages[1].at<double>(i, j) = BlEstimate2(imagesCopy[1], i,
                                                             j);
        }

        if (edges[3].at<uint8_t>(i, j)) {
          firstStageImages[3].at<double>(i, j) = CovEstimate(imagesCopy[3], i,
                                                             j,
                                                             true);
        } else {
          firstStageImages[3].at<double>(i, j) = BlEstimate2(imagesCopy[3], i,
                                                             j);
        }
      } else if (isHeightOdd == 0 && isWidthOdd == 1) {// %45
        if (edges[0].at<uint8_t>(i, j)) {
          firstStageImages[0].at<double>(i, j) = CovEstimate(imagesCopy[0], i,
                                                             j,
                                                             true);
        } else {
          firstStageImages[0].at<double>(i, j) = BlEstimate2(imagesCopy[0], i,
                                                             j);
        }

        if (edges[2].at<uint8_t>(i, j)) {
          firstStageImages[2].at<double>(i, j) = CovEstimate(imagesCopy[2], i,
                                                             j,
                                                             true);
        } else {
          firstStageImages[2].at<double>(i, j) = BlEstimate2(imagesCopy[2], i,
                                                             j);
        }
      } else if (isHeightOdd == 0 && isWidthOdd == 0) { // %90
        if (edges[1].at<uint8_t>(i, j)) {
          firstStageImages[1].at<double>(i, j) = CovEstimate(imagesCopy[1], i,
                                                             j,
                                                             true);
        } else {
          firstStageImages[1].at<double>(i, j) = BlEstimate2(imagesCopy[1], i,
                                                             j);
        }

        if (edges[3].at<uint8_t>(i, j)) {
          firstStageImages[3].at<double>(i, j) = CovEstimate(imagesCopy[3], i,
                                                             j,
                                                             true);
        } else {
          firstStageImages[3].at<double>(i, j) = BlEstimate2(imagesCopy[3], i,
                                                             j);
        }
      } else { // 135
        if (edges[0].at<uint8_t>(i, j)) {
          firstStageImages[0].at<double>(i, j) = CovEstimate(imagesCopy[0], i,
                                                             j,
                                                             true);
        } else {
          firstStageImages[0].at<double>(i, j) = BlEstimate2(imagesCopy[0], i,
                                                             j);
        }

        if (edges[2].at<uint8_t>(i, j)) {
          firstStageImages[2].at<double>(i, j) = CovEstimate(imagesCopy[2], i,
                                                             j,
                                                             true);
        } else {
          firstStageImages[2].at<double>(i, j) = BlEstimate2(imagesCopy[2], i,
                                                             j);
        }
      }
    }
  }
}