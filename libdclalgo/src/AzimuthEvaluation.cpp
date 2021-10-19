#include "AzimuthEvaluation.h"
#include "LibraryParameters.h"

#include <cmath>
#include <opencv2/core/utility.hpp>

cv::Mat getAtan2Vector(cv::Mat m1, cv::Mat m2) {
  cv::Mat res(m1.cols, 1, CV_64FC1);

  for (int j = 0; j < m1.cols; ++j) {
    res.at<double>(j) = atan2(m2.at<double>(j), m1.at<double>(j));
  }
  return res;
}

static cv::Mat getAtan2Vector_fast_O(const cv::Mat &O) {
  cv::Mat res(O.cols, 1, CV_64FC1);

  for (int j = 0; j < O.cols; ++j) {
    res.at<double>(j) = atan2(O.at<double>(2, j), O.at<double>(1, j)) * 0.5;
  }
  return res;
}

class parallelAtan2Vector : public cv::ParallelLoopBody {
private:
  cv::Mat *m1;
  cv::Mat *m2;
  cv::Mat *result;

public:
  parallelAtan2Vector(cv::Mat *_m1, cv::Mat *_m2, cv::Mat *_res)
      : m1(_m1), m2(_m2), result(_res) {}

  virtual void operator()(const cv::Range &range) const {
    for (int i = range.start; i != range.end; ++i) {
      result->at<double>(i) = atan2(m2->at<double>(i), m1->at<double>(i));
    }
  }
};

cv::Mat getAtan2VectorParallel(cv::Mat m1, cv::Mat m2) {
  cv::Mat result(std::min(m1.cols, m2.cols), 1, CV_64FC1);
  cv::parallel_for_(cv::Range(0, m1.cols),
                    parallelAtan2Vector(&m1, &m2, &result));
  return result;
}

template <typename T> void normMat(cv::Mat &m) {
  auto p = pow(2, sizeof(T)) - 1;
  double min, max;
  cv::minMaxLoc(m, &min, &max);
  m = p * (m - min) / (max - min);
}

// (!!!) We have decided not to use this function because cv::invert method
// causes errors in angles (azimuth, zenith)
//
// cv::Mat getPolarMatrix(const std::vector<double> &polarAngles) {
//  cv::Mat P(polarAngles.size(), 3, CV_64F);
//
//  for (size_t i = 0; i < polarAngles.size(); ++i) {
//    P.at<double>(i, 0) = 1.;
//    P.at<double>(i, 1) = cos(MPI_90 * polarAngles[i]);
//    P.at<double>(i, 2) = sin(MPI_90 * polarAngles[i]);
//  }
//
//  cv::Mat PInverse;
//  cv::invert(P, PInverse, cv::DECOMP_SVD);
//  return PInverse;
//}

cv::Mat getReshapedPolarImages(const std::vector<cv::Mat> &polarImages) {
  cv::Mat D;
  for (size_t j = 0; j < polarImages.size(); j++) {
    D.push_back(polarImages[j].reshape(0, 1));
  }
  return D;
}

cv::Mat getReshapedPolarImages_fast(const std::vector<cv::Mat> &polarImages) {
  cv::Mat D(polarImages.size(), polarImages.front().total(),
            polarImages.front().type());
  auto imgs_type = polarImages.front().type();
  size_t max_i = polarImages.size();
  cv::Mat tmp;
  switch (imgs_type) {
  case CV_8U:
    for (size_t i = 0; i < max_i; ++i) {
      tmp = polarImages[i].reshape(0, 1);
      for (int j = 0; j < tmp.cols; ++j) {
        D.at<uchar>(i, j) = tmp.at<uchar>(j);
      }
    }
    break;
  case CV_8S:
    for (size_t i = 0; i < max_i; ++i) {
      tmp = polarImages[i].reshape(0, 1);
      for (int j = 0; j < tmp.cols; ++j) {
        D.at<char>(i, j) = tmp.at<char>(j);
      }
    }
    break;
  case CV_16S:
    for (size_t i = 0; i < max_i; ++i) {
      tmp = polarImages[i].reshape(0, 1);
      for (int j = 0; j < tmp.cols; ++j) {
        D.at<short>(i, j) = tmp.at<short>(j);
      }
    }
    break;
  case CV_16U:
    for (size_t i = 0; i < max_i; ++i) {
      tmp = polarImages[i].reshape(0, 1);
      for (int j = 0; j < tmp.cols; ++j) {
        D.at<ushort>(i, j) = tmp.at<ushort>(j);
      }
    }
    break;
  case CV_32S:
    for (size_t i = 0; i < max_i; ++i) {
      tmp = polarImages[i].reshape(0, 1);
      for (int j = 0; j < tmp.cols; ++j) {
        D.at<int>(i, j) = tmp.at<int>(j);
      }
    }
    break;
  case CV_32F:
    for (size_t i = 0; i < max_i; ++i) {
      tmp = polarImages[i].reshape(0, 1);
      for (int j = 0; j < tmp.cols; ++j) {
        D.at<float>(i, j) = tmp.at<float>(j);
      }
    }
    break;
  case CV_64F:
    for (size_t i = 0; i < max_i; ++i) {
      tmp = polarImages[i].reshape(0, 1);
      for (int j = 0; j < tmp.cols; ++j) {
        D.at<double>(i, j) = tmp.at<double>(j);
      }
    }
    break;
  default:
    return cv::Mat();
  }
  return D;
}

cv::Mat computeO(const std::vector<cv::Mat> &polarImages) {
  cv::Mat D = getReshapedPolarImages(polarImages);
  auto Pinv_array = LibraryParameters::getPolarMatrix();
  cv::Mat Pinv = cv::Mat(cv::Size(4, 3), CV_64FC1, Pinv_array.data());
  D.convertTo(D, CV_64F);
  return Pinv * D;
}

void computeAzimuth(const std::vector<cv::Mat> &polarImages, cv::Mat &azimuth) {
  cv::Mat O = computeO(polarImages);
  cv::Mat Phi = getAtan2Vector(O.row(1), O.row(2)) / 2.;
  azimuth = Phi.reshape(0, polarImages[0].rows);
}

void computeAzimuth_fast(const std::vector<cv::Mat> &polarImages,
                         cv::Mat &azimuth) {
  auto Pinv_array = LibraryParameters::getPolarMatrix();
  cv::Mat Pinv = cv::Mat(cv::Size(4, 3), CV_64FC1, Pinv_array.data());
  cv::Mat D = getReshapedPolarImages_fast(polarImages);
  D.convertTo(D, CV_64F);
  cv::Mat O;
  cv::gemm(Pinv, D, 1, cv::Mat(), 0, O, 0);
  cv::Mat Phi = getAtan2Vector_fast_O(O);
  azimuth = Phi.reshape(0, polarImages[0].rows);
}

void computePolDegree(const std::vector<cv::Mat> &polarImages,
                      cv::Mat &polDegrees) {
  cv::Mat O = computeO(polarImages);
  cv::Mat Rho;
  cv::sqrt((O.row(1).mul(O.row(1)) + O.row(2).mul(O.row(2))),
           Rho); //^0.5 / O.row(0);
  Rho = Rho / O.row(0);
  std::transform(Rho.begin<double>(), Rho.end<double>(), Rho.begin<double>(),
                 [](const double &el) -> double {
                   double result =
                       el < std::numeric_limits<double>::epsilon() ? 0.0 : el;
                   return result > 1.0 ? 1.0 : result;
                 });
  polDegrees = Rho.reshape(0, polarImages[0].rows);
}