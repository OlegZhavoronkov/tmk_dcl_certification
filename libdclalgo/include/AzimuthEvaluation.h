#ifndef AZIMUTH_EVALUATION_H
#define AZIMUTH_EVALUATION_H

#include "dclalgo_export.h"

#include <opencv2/core/mat.hpp>
#include <vector>

DCLALGO_EXPORT cv::Mat getAtan2Vector(cv::Mat m1, cv::Mat m2);
DCLALGO_EXPORT cv::Mat getAtan2VectorParallel(cv::Mat m1, cv::Mat m2);
DCLALGO_EXPORT cv::Mat
getReshapedPolarImages(const std::vector<cv::Mat> &polarImages);
DCLALGO_EXPORT cv::Mat
getReshapedPolarImages_fast(const std::vector<cv::Mat> &polarImages);

template <typename T>
DCLALGO_EXPORT cv::Mat
getReshapedPolarImages_fast_template(const std::vector<cv::Mat> &polarImages) {
  cv::Mat D(int(polarImages.size()), polarImages.front().total(),
            polarImages.front().type());
  for (size_t i = 0; i < polarImages.size(); ++i) {
    cv::Mat tmp = polarImages[i].reshape(0, 1);
    for (int j = 0; j < tmp.cols; ++j)
      D.at<T>(int(i), j) = tmp.at<T>(j);
  }
  return D;
}

DCLALGO_EXPORT cv::Mat computeO(const std::vector<cv::Mat> &polarImages);
DCLALGO_EXPORT void computeAzimuth(const std::vector<cv::Mat> &polarImages,
                                   cv::Mat &azimuth);
DCLALGO_EXPORT void computeAzimuth_fast(const std::vector<cv::Mat> &polarImages,
                                        cv::Mat &azimuth);
DCLALGO_EXPORT void computePolDegree(const std::vector<cv::Mat> &polarImages,
                                     cv::Mat &polDegrees);
#endif // AZIMUTH_EVALUATION_H
