#include "PolarDecomposition.h"

#include <gtest/gtest.h>
#include <opencv2/core.hpp>

cv::Mat epsilon(int rows, int cols) {
  // Epsilon for double values 1e-15,
  // std::numeric_limits<double>::epsilon() is not good way,
  // close values don't fit
  return cv::Mat(rows, cols, CV_64FC1, 1e-15);
}

TEST(PolarizationAlgorithmsDecomposition, diffuseModelZero) {
    cv::Mat x(3, 3, CV_32SC1, 1);
    cv::Mat res = diffuseModel(1, x);
    EXPECT_EQ(cv::countNonZero(res), 0);
}

TEST(PolarizationAlgorithmsDecomposition, diffuseModel33) {
    cv::Mat x(3, 3, CV_64FC1, 1);
    cv::Mat res = diffuseModel(2, x);
    cv::Mat expected(3, 3, CV_64FC1, 0.1677763528232861);
    EXPECT_EQ(cv::countNonZero(cv::abs(res - expected) > epsilon(3, 3)), 0);
}

TEST(PolarizationAlgorithmsDecomposition, diffuseModel32) {
    cv::Mat x(3, 2, CV_64FC1, 1);
    cv::Mat res = diffuseModel(2, x);
    cv::Mat expected(3, 2, CV_64FC1, 0.1677763528232861);
    EXPECT_EQ(cv::countNonZero(cv::abs(res - expected) > epsilon(3, 2)), 0);
}

TEST(PolarizationAlgorithmsDecomposition, speculareModel33) {
    cv::Mat x(3, 3, CV_64FC1, 1);
    cv::Mat res = specularModel(2, x);
    cv::Mat expected(3, 3, CV_64FC1, 0.9493213613639827);
    EXPECT_EQ(cv::countNonZero(cv::abs(res - expected) > epsilon(3, 3)), 0);
}

TEST(PolarizationAlgorithmsDecomposition, specularModel32) {
    cv::Mat x(3, 2, CV_64FC1, 1);
    cv::Mat res = specularModel(2, x);
    cv::Mat expected(3, 2, CV_64FC1, 0.9493213613639827);
    EXPECT_EQ(cv::countNonZero(cv::abs(res - expected) > epsilon(3, 2)), 0);
}

TEST(PolarizationAlgorithmsDecomposition, pixelModelZeros) {
    cv::Mat x(1,1,CV_32SC1, (int)0);
    cv::Mat res = pixelModel(x, x, x, x);
    EXPECT_EQ(cv::countNonZero(res), 0);
}

TEST(PolarizationAlgorithmsDecomposition, pixelModelOnes) {
    cv::Mat x(1,1,CV_32SC1, (int)1);
    cv::Mat res = pixelModel(x, x, x, x);
    EXPECT_EQ(cv::countNonZero(res - 1), 0);
}

TEST(PolarizationAlgorithmsDecomposition, pixelModelOneZero) {
    cv::Mat x(1,1,CV_64FC1, 0.);
    cv::Mat y(1,1,CV_64FC1, 1);
    cv::Mat res = pixelModel(x, y, x, y);
    EXPECT_EQ(cv::countNonZero((res - 0.7080734182735712) > epsilon(1, 1)), 0);
    res = pixelModel(y, x, y, x);
    EXPECT_EQ(cv::countNonZero((res - 0.2919265817264288) > epsilon(1, 1)), 0);
}

TEST(PolarizationAlgorithmsDecomposition, iMin) {
    cv::Mat x(1,1,CV_64FC1, 1);
    cv::Mat y(1,1,CV_64FC1, 3);
    cv::Mat res = iMin(x, x);
    EXPECT_EQ(cv::countNonZero(res), 0);
    res = iMin(x, y);;
    EXPECT_EQ(cv::countNonZero((res + 0.5) > epsilon(1, 1)), 0);
    res = iMin(y, y);;
    EXPECT_EQ(cv::countNonZero((res + 1.5) > epsilon(1, 1)), 0);
}

TEST(PolarizationAlgorithmsDecomposition, decomposeImages) {
    cv::Mat x(1,1,CV_64FC1, 1);
    std::vector<cv::Mat> res = decomposeImages(x, x, x, {0});
    EXPECT_EQ(res.size(), 1);
    EXPECT_EQ(cv::countNonZero((res[0] - 0.2919265817264288) > epsilon(1, 1)), 0);
}