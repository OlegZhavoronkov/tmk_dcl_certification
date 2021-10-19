#include "PolarExpand.h"

#include <gtest/gtest.h>
#include <iostream>
#include <limits>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

class PolarExpandTests : public ::testing::Test{
public:
  PolarExpandTests();
protected:
  cv::Size imageSize;

  cv::Mat data8Bit;
  cv::Mat data16Bit;

  cv::Mat data8BitC00;
  cv::Mat data8BitC10;
  cv::Mat data8BitC01;
  cv::Mat data8BitC11;

  cv::Mat data16BitC00;
  cv::Mat data16BitC10;
  cv::Mat data16BitC01;
  cv::Mat data16BitC11;

};

namespace {
template<typename T>
cv::Mat genData(const cv::Size &size, const std::array<T, 4> &samples) {
  int dataType = CV_8UC1;
  int ss = sizeof(T);
  if(ss == 2)
    dataType = CV_16UC1;
  cv::Mat test = cv::Mat(size, dataType);
  for (int y = 0; y < size.height - 1; y += 2) {
    for (int x = 0; x < size.width - 1; x += 2) {
      test.at<T>(y, x) = samples[0];
      test.at<T>(y + 1, x) = samples[1];
      test.at<T>(y, x + 1) = samples[2];
      test.at<T>(y + 1, x + 1) = samples[3];
    }
  }
  return test;
}
}

PolarExpandTests::PolarExpandTests()
    : imageSize(2248, 2048)
    , data8Bit(genData<uint8_t >(imageSize, {10, 100, 50, 150}))
    , data16Bit(genData<uint16_t >(imageSize, {0, 10000, 20000, 30000}))

    , data8BitC00(imageSize, CV_8UC1, 10)
    , data8BitC10(imageSize, CV_8UC1, 100)
    , data8BitC01(imageSize, CV_8UC1, 50)
    , data8BitC11(imageSize, CV_8UC1, 150)

    , data16BitC00(imageSize, CV_16UC1, 0.0)
    , data16BitC10(imageSize, CV_16UC1, 10000)
    , data16BitC01(imageSize, CV_16UC1, 20000)
    , data16BitC11(imageSize, CV_16UC1, 30000){

}

TEST_F(PolarExpandTests, expand8bit)
{
    PolarExpand<uint8_t> expander;
    std::vector<cv::Mat> result;
    expander.expand(data8Bit, result);
    ASSERT_EQ(cv::mean(cv::abs(result[0] - data8BitC11)(cv::Rect(0, 0, imageSize.width - 1, imageSize.height - 1)))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[1] - data8BitC01)(cv::Rect(0, 0, imageSize.width - 1, imageSize.height - 1)))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[2] - data8BitC00)(cv::Rect(0, 0, imageSize.width - 1, imageSize.height - 1)))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[3] - data8BitC10)(cv::Rect(0, 0, imageSize.width - 1, imageSize.height - 1)))[0], 0);
}

TEST_F(PolarExpandTests, expand16bit)
{
    PolarExpand<uint16_t> expander;
    std::vector<cv::Mat> result;
    expander.expand(data16Bit, result);
    ASSERT_EQ(cv::mean(cv::abs(result[0] - data16BitC11)(cv::Rect(0, 0, imageSize.width - 1, imageSize.height - 1)))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[1] - data16BitC01)(cv::Rect(0, 0, imageSize.width - 1, imageSize.height - 1)))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[2] - data16BitC00)(cv::Rect(0, 0, imageSize.width - 1, imageSize.height - 1)))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[3] - data16BitC10)(cv::Rect(0, 0, imageSize.width - 1, imageSize.height - 1)))[0], 0);
}

TEST_F(PolarExpandTests, expandEasy8bit)
{
    PolarExpand<uint8_t> expander;
    std::vector<cv::Mat> result;
    expander.expandEasy(data8Bit, result);
    ASSERT_EQ(cv::mean(cv::abs(result[0] - data8BitC11(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[1] - data8BitC10(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[2] - data8BitC00(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[3] - data8BitC01(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
}

TEST_F(PolarExpandTests, expandEasy16bit)
{
    PolarExpand<uint16_t> expander;
    std::vector<cv::Mat> result;
    expander.expandEasy(data16Bit, result);
    ASSERT_EQ(cv::mean(cv::abs(result[0] - data16BitC11(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[1] - data16BitC10(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[2] - data16BitC00(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[3] - data16BitC01(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
}

TEST_F(PolarExpandTests, expandDebayer8bit)
{
    PolarExpand<uint8_t> expander;
    std::vector<cv::Mat> result;
    expander.expandDebayer(data8Bit, result);
    ASSERT_EQ(cv::mean(cv::abs(result[0] - data8BitC11))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[1] - data8BitC01))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[2] - data8BitC00))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[3] - data8BitC10))[0], 0);
}

TEST_F(PolarExpandTests, expandDebayer16bit)
{
    PolarExpand<uint16_t> expander;
    std::vector<cv::Mat> result;
    expander.expandDebayer(data16Bit, result);
    ASSERT_EQ(cv::mean(cv::abs(result[0] - data16BitC11))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[1] - data16BitC01))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[2] - data16BitC00))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[3] - data16BitC10))[0], 0);
}

TEST_F(PolarExpandTests, expandDebayerFast8bit)
{
    PolarExpand<uint8_t> expander;
    std::vector<cv::Mat> result;
    expander.expandDebayer_fast(data8Bit, result);
    ASSERT_EQ(cv::mean(cv::abs(result[0] - data8BitC11))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[1] - data8BitC01))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[2] - data8BitC00))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[3] - data8BitC10))[0], 0);
}

TEST_F(PolarExpandTests, expandDebayerFast16bit)
{
    PolarExpand<uint16_t> expander;
    std::vector<cv::Mat> result;
    expander.expandDebayer_fast(data16Bit, result);
    ASSERT_EQ(cv::mean(cv::abs(result[0] - data16BitC11))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[1] - data16BitC01))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[2] - data16BitC00))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[3] - data16BitC10))[0], 0);
}

TEST_F(PolarExpandTests, expandLite8bit)
{
    PolarExpand<uint8_t> expander;
    std::vector<cv::Mat> result;
    expander.expandLite(data8Bit, result);
    ASSERT_EQ(cv::mean(cv::abs(result[0] - data8BitC11(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[1] - data8BitC10(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[2] - data8BitC00(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[3] - data8BitC01(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
}

TEST_F(PolarExpandTests, expandLite16bit)
{
    PolarExpand<uint16_t> expander;
    std::vector<cv::Mat> result;
    expander.expandLite(data16Bit, result);
    ASSERT_EQ(cv::mean(cv::abs(result[0] - data16BitC11(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[1] - data16BitC10(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[2] - data16BitC00(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
    ASSERT_EQ(cv::mean(cv::abs(result[3] - data16BitC01(cv::Rect(0, 0, imageSize.width / 2, imageSize.height / 2))))[0], 0);
}
