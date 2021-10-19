#include "ZenithEvaluation.h"
#include "MapToMat.h"

#include <gtest/gtest.h>

#include <iostream>
#include <limits>

#include <opencv2/core.hpp>


class ZenithEvaluationTests : public ::testing::Test {
public:
  ZenithEvaluationTests();

  cv::Mat genRandomMat(double min, double max);

protected:
  cv::Mat dop;
  cv::Mat thetaDiffuse;
  std::vector<double> dopValues = {0, 1, 2, 3, 4, 5};
  std::vector<double> thetaDiffuseValues = {0, 1.06369782240256,
                                            1.03252336906157, 1.02042853531149,
                                            1.013975450760647,
                                            1.009958599773045};

  cv::Mat specularRefl11;
  cv::Mat specularRefl13;
  cv::Mat specularRefl18;
  std::vector<double> specularReflData11 = {0, 0.8366840002995264,
                                            -0.567978686342833,
                                            -0.03686706924056881,
                                            -0.9956740515529682,
                                            0.3235695181659888};
  std::vector<double> specularReflData13 = {0, 0.9621620822392044,
                                            -0.7674184901394192,
                                            -0.03112438330153156,
                                            -0.9826669805646848,
                                            0.5045568780882317};
  std::vector<double> specularReflData18 = {0, 0.9814592429938036,
                                            -0.9704682024560135,
                                            -0.02241749218218506,
                                            -0.8332069151137348,
                                            0.7698360367007047};
};

ZenithEvaluationTests::ZenithEvaluationTests() {
  cv::theRNG().state = time(nullptr);

  dop = cv::Mat(1, dopValues.size(), CV_64FC1, dopValues.data());
  thetaDiffuse = cv::Mat(1, thetaDiffuseValues.size(), CV_64FC1,
                         thetaDiffuseValues.data());

  specularRefl11 = cv::Mat(1, specularReflData11.size(), CV_64FC1,
                           specularReflData11.data());
  specularRefl13 = cv::Mat(1, specularReflData13.size(), CV_64FC1,
                           specularReflData13.data());
  specularRefl18 = cv::Mat(1, specularReflData18.size(), CV_64FC1,
                           specularReflData18.data());
}


static cv::Mat epsilon(int rows, int cols) {
  // Epsilon for double values 1e-15,
  // std::numeric_limits<double>::epsilon() is not good way,
  // close values don't fit
  return cv::Mat(rows, cols, CV_64FC1, 1e-15);
}

TEST_F(ZenithEvaluationTests, zenithDiffuse) {
  cv::Mat result = zenith_eval_diffuse(dop, 1.8);
  EXPECT_EQ(cv::countNonZero(
      cv::abs(result - thetaDiffuse) > (epsilon(dop.rows, dop.cols) * 10)), 0);
  result = zenith_eval_diffuse_fast(dop, 1.8);
  EXPECT_EQ(cv::countNonZero(
      cv::abs(result - thetaDiffuse) > (epsilon(dop.rows, dop.cols) * 10)), 0);
}

TEST_F(ZenithEvaluationTests, specularReflectionModel) {
  cv::Mat result = mapToMat<double>(dop, [](double x) -> double {
    return specularReflectionModel(1.1, x);
  });
  EXPECT_EQ(cv::countNonZero(cv::abs(result - specularRefl11) >
                             epsilon(specularRefl11.rows, specularRefl11.cols)),
            0);
  result = mapToMat<double>(dop, [](double x) -> double {
    return specularReflectionModel(1.3, x);
  });
  EXPECT_EQ(cv::countNonZero(cv::abs(result - specularRefl13) >
                             epsilon(specularRefl13.rows, specularRefl13.cols)),
            0);
  result = mapToMat<double>(dop, [](double x) -> double {
    return specularReflectionModel(1.8, x);
  });
  EXPECT_EQ(cv::countNonZero(cv::abs(result - specularRefl18) >
                             epsilon(specularRefl11.rows, specularRefl18.cols)),
            0);
}

TEST_F(ZenithEvaluationTests, argmax) {
  for (double n = 1.1; n < 2.0; n += 0.01)
    EXPECT_TRUE(specularReflectionModel(n, argmax(n)) - 1.0 <
                std::numeric_limits<double>::epsilon());
}

TEST_F(ZenithEvaluationTests, zenith_eval_specular) {
  cv::Mat result = zenith_eval_specular(cv::Mat(1, 1, CV_64FC1, 0.5), 1.1,
                                        2000);
  cv::Mat baseResult = cv::Mat(1, 1, CV_64FC2,
                               {0.5027589834778434,
                                1.1778545882128575});
  cv::Mat eps = cv::Mat(1, 1, CV_64FC2, {std::numeric_limits<double>::epsilon(),
                                         std::numeric_limits<double>::epsilon()});

  // Adjust to cv::countNonZero function: assert cn == 1.
  result = result.reshape(1);
  baseResult = baseResult.reshape(1);
  eps = eps.reshape(1);
  EXPECT_EQ(cv::countNonZero(cv::abs(result - baseResult) > eps), 0);
}
