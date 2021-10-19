#include "AzimuthEvaluation.h"
#include "PolarDecomposition.h"
#include "LibraryParameters.h"

#include <opencv2/core.hpp>
#include <gtest/gtest.h>

#include <limits>
#include <vector>

class AzimuthEvaluationTests : public ::testing::Test {
public:
  AzimuthEvaluationTests();

  cv::Mat genRandomMat(double min, double max);

protected:
  cv::Mat x;
  cv::Mat y;
  cv::Mat atan2XYAnswer;

  std::vector<double> polarAngles = {0, 45, 90, 135};
  std::vector<double> pInverseMatrixData = {0.2500, 0.2500, 0.2500, 0.2500,
                                            0.5000, 0.0000, -0.5000, 0.0000,
                                            0.0000, 0.5000, 0.0000, -0.5000};
  cv::Mat pInverseMatrix;

  std::vector<uint8_t> imageData1 =
      {
          1, 2, 3,
          4, 5, 6
      };

  std::vector<uint8_t> imageData2 =
      {
          11, 12, 13,
          14, 15, 16
      };
  std::vector<uint8_t> combinedImageData =
      {
          1, 2, 3, 4, 5, 6,
          11, 12, 13, 14, 15, 16,
      };
  cv::Mat image1;
  cv::Mat image2;
  cv::Mat combinedImage;

  int rows = 2048;
  int cols = 2448;

  double reflection = 1.3;

  std::vector<double> standardAnglesRad = {0, M_PI / 4, M_PI / 2, M_PI * 3 / 4};
};

AzimuthEvaluationTests::AzimuthEvaluationTests() {
  cv::theRNG().state = time(nullptr);

  double xData = 1. / sqrt(2.);
  double yData = 1. / sqrt(2.);

  double atan2Answer = atan2(yData, xData);
  int vectorSize = 10000000;
  x = cv::Mat(1, vectorSize, CV_64FC1, xData);
  y = cv::Mat(1, vectorSize, CV_64FC1, yData);
  atan2XYAnswer = cv::Mat(vectorSize, 1, CV_64FC1, atan2Answer);

  pInverseMatrix = cv::Mat(3, 4, CV_64FC1, pInverseMatrixData.data());

  image1 = cv::Mat(2, 3, CV_8UC1, imageData1.data());
  image2 = cv::Mat(2, 3, CV_8UC1, imageData2.data());
  combinedImage = cv::Mat(2, 6, CV_8UC1, combinedImageData.data());

  LibraryParameters::configure(LibraryParameters::PolarizationSensorOrder::order90_45_135_0);
}

cv::Mat AzimuthEvaluationTests::genRandomMat(double min, double max) {
  cv::Mat dst(rows, cols, CV_64FC1, 0.);
  cv::randu(dst, min, max);
  return dst;
}

static cv::Mat epsilon(int rows, int cols) {
  // Epsilon for double values 1e-15,
  // std::numeric_limits<double>::epsilon() is not good way,
  // close values don't fit
  return cv::Mat(rows, cols, CV_64FC1, 1e-15);
}

TEST_F(AzimuthEvaluationTests, getAtan2Vector) {
  cv::Mat atanRes = getAtan2Vector(x, y);
  EXPECT_EQ(cv::countNonZero(atan2XYAnswer != atanRes), 0);
}

TEST_F(AzimuthEvaluationTests, getAtan2VectorParallel) {
  cv::Mat atanRes = getAtan2VectorParallel(x, y);
  EXPECT_EQ(cv::countNonZero(atan2XYAnswer != atanRes), 0);
}

TEST_F(AzimuthEvaluationTests, getReshapedPolarImagesFast) {
  cv::Mat p = getReshapedPolarImages_fast({image1, image2});
  EXPECT_EQ(cv::countNonZero(combinedImage != p), 0);
}

TEST_F(AzimuthEvaluationTests, getReshapedPolarImagesFastTemplate) {
  cv::Mat p = getReshapedPolarImages_fast_template<uint8_t>({image1, image2});
  EXPECT_EQ(cv::countNonZero(combinedImage != p), 0);
}

TEST_F(AzimuthEvaluationTests, computeAzimuth) {
  cv::Mat x(1, 2, CV_64FC1, 1.3);
  std::vector<cv::Mat> polarImages = decomposeImages(x, x, x,
                                                     standardAnglesRad);
  cv::Mat res(1, 2, CV_64FC1, 0.);
  computeAzimuth(polarImages, res);
  EXPECT_EQ(cv::countNonZero(cv::abs(res - x) > epsilon(1, 2)), 0);
}

TEST_F(AzimuthEvaluationTests, computeAzimuthFast) {
  cv::Mat x(1, 2, CV_64FC1, 1.3);
  std::vector<cv::Mat> polarImages = decomposeImages(x, x, x,
                                                     standardAnglesRad);
  cv::Mat res(1, 2, CV_64FC1, 0.);
  computeAzimuth_fast(polarImages, res);
  EXPECT_EQ(cv::countNonZero(cv::abs(res - x) > epsilon(1, 2)), 0);
}

TEST_F(AzimuthEvaluationTests, computePolDegree) {
  cv::Mat x(1, 2, CV_64FC1, 1.0);
  std::vector<cv::Mat> polarImages = decomposeImages(x, x, x,
                                                     standardAnglesRad);
  cv::Mat res(1, 2, CV_64FC1, 0.);
  computePolDegree(polarImages, res);
  EXPECT_EQ(cv::countNonZero(cv::abs(res - x) > epsilon(1, 2)), 0);
}
