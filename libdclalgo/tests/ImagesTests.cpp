#include "AzimuthEvaluation.h"
#include "LibraryParameters.h"
#include "PolarExpand.h"
#include "ZenithEvaluation.h"

#include <gtest/gtest.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include <chrono>
#include <limits>
#include <string>

class ImagesTests : public ::testing::Test {
public:
  ImagesTests();

protected:
  const std::string _testDataFolder = TEST_DATA_FOLDER;
  std::string pathToOriginalImage = _testDataFolder + "/def1_SPN0.bmp";
  std::string pathToAzimuthMap = _testDataFolder + "/azimuthMap.yml";
  std::string pathToZenithMapReflectionDiffuse =
      _testDataFolder + "/zenithMapReflectionDiffuse.yml";
  std::string pathToZenithMapReflectionSpecular =
      _testDataFolder + "/zenithMapReflectionSpecular.yml";
  std::string pathToPolarDegreeMap = _testDataFolder + "/polarDegreeMap.yml";

  std::string pathToExpand_1 = _testDataFolder + "/def1_SPN0_expand_1.tiff";
  std::string pathToExpand_2 = _testDataFolder + "/def1_SPN0_expand_2.tiff";
  std::string pathToExpand_3 = _testDataFolder + "/def1_SPN0_expand_3.tiff";
  std::string pathToExpand_4 = _testDataFolder + "/def1_SPN0_expand_4.tiff";

  std::string pathToExpandDebayer_1 =
      _testDataFolder + "/def1_SPN0_expandDebayer_1.tiff";
  std::string pathToExpandDebayer_2 =
      _testDataFolder + "/def1_SPN0_expandDebayer_2.tiff";
  std::string pathToExpandDebayer_3 =
      _testDataFolder + "/def1_SPN0_expandDebayer_3.tiff";
  std::string pathToExpandDebayer_4 =
      _testDataFolder + "/def1_SPN0_expandDebayer_4.tiff";

  std::string pathToExpandDebayer_fast_1 =
      _testDataFolder + "/def1_SPN0_expandDebayer_fast_1.tiff";
  std::string pathToExpandDebayer_fast_2 =
      _testDataFolder + "/def1_SPN0_expandDebayer_fast_2.tiff";
  std::string pathToExpandDebayer_fast_3 =
      _testDataFolder + "/def1_SPN0_expandDebayer_fast_3.tiff";
  std::string pathToExpandDebayer_fast_4 =
      _testDataFolder + "/def1_SPN0_expandDebayer_fast_4.tiff";

  std::string pathToExpandDebayerWithoutBoundaries_1 =
      _testDataFolder + "/def1_SPN0_expandDebayerWithoutBoundaries_1.tiff";
  std::string pathToExpandDebayerWithoutBoundaries_2 =
      _testDataFolder + "/def1_SPN0_expandDebayerWithoutBoundaries_2.tiff";
  std::string pathToExpandDebayerWithoutBoundaries_3 =
      _testDataFolder + "/def1_SPN0_expandDebayerWithoutBoundaries_3.tiff";
  std::string pathToExpandDebayerWithoutBoundaries_4 =
      _testDataFolder + "/def1_SPN0_expandDebayerWithoutBoundaries_4.tiff";

  std::string pathToExpandEasy_1 =
      _testDataFolder + "/def1_SPN0_expandEasy_1.tiff";
  std::string pathToExpandEasy_2 =
      _testDataFolder + "/def1_SPN0_expandEasy_2.tiff";
  std::string pathToExpandEasy_3 =
      _testDataFolder + "/def1_SPN0_expandEasy_3.tiff";
  std::string pathToExpandEasy_4 =
      _testDataFolder + "/def1_SPN0_expandEasy_4.tiff";

  std::string pathToExpandLite_1 =
      _testDataFolder + "/def1_SPN0_expandLite_1.tiff";
  std::string pathToExpandLite_2 =
      _testDataFolder + "/def1_SPN0_expandLite_2.tiff";
  std::string pathToExpandLite_3 =
      _testDataFolder + "/def1_SPN0_expandLite_3.tiff";
  std::string pathToExpandLite_4 =
      _testDataFolder + "/def1_SPN0_expandLite_4.tiff";

  cv::Mat originalImage;
  cv::Mat expandEtalon_1;
  cv::Mat expandEtalon_2;
  cv::Mat expandEtalon_3;
  cv::Mat expandEtalon_4;

  cv::Mat expandDebayerEtalon_1;
  cv::Mat expandDebayerEtalon_2;
  cv::Mat expandDebayerEtalon_3;
  cv::Mat expandDebayerEtalon_4;

  cv::Mat expandDebayer_fastEtalon_1;
  cv::Mat expandDebayer_fastEtalon_2;
  cv::Mat expandDebayer_fastEtalon_3;
  cv::Mat expandDebayer_fastEtalon_4;

  cv::Mat expandDebayerWithoutBoundariesEtalon_1;
  cv::Mat expandDebayerWithoutBoundariesEtalon_2;
  cv::Mat expandDebayerWithoutBoundariesEtalon_3;
  cv::Mat expandDebayerWithoutBoundariesEtalon_4;

  cv::Mat expandEasyEtalon_1;
  cv::Mat expandEasyEtalon_2;
  cv::Mat expandEasyEtalon_3;
  cv::Mat expandEasyEtalon_4;

  cv::Mat expandLiteEtalon_1;
  cv::Mat expandLiteEtalon_2;
  cv::Mat expandLiteEtalon_3;
  cv::Mat expandLiteEtalon_4;

  cv::Mat azimuthMapEtalon;
  cv::Mat zenithMapReflectionDiffuseEtalon;
  cv::Mat zenithMapReflectionSpecularEtalon;
  cv::Mat polarDegreeMapEtalon;

  std::vector<cv::Mat> expandedSource;

  const float zenithN = 1.3;
  const int zenithPointsNumber = 2000;
};

ImagesTests::ImagesTests() {
  originalImage = cv::imread(pathToOriginalImage, cv::IMREAD_ANYDEPTH |
                                                  cv::IMREAD_ANYCOLOR);

  expandEtalon_1 = cv::imread(pathToExpand_1,
                              cv::IMREAD_ANYDEPTH |
                              cv::IMREAD_ANYCOLOR);
  expandEtalon_2 = cv::imread(pathToExpand_2,
                              cv::IMREAD_ANYDEPTH |
                              cv::IMREAD_ANYCOLOR);
  expandEtalon_3 = cv::imread(pathToExpand_3,
                              cv::IMREAD_ANYDEPTH |
                              cv::IMREAD_ANYCOLOR);
  expandEtalon_4 = cv::imread(pathToExpand_4,
                              cv::IMREAD_ANYDEPTH |
                              cv::IMREAD_ANYCOLOR);

  expandedSource = {expandEtalon_1, expandEtalon_2,
                    expandEtalon_3, expandEtalon_4};

  expandDebayerEtalon_1 = cv::imread(pathToExpandDebayer_1,
                                     cv::IMREAD_ANYDEPTH |
                                     cv::IMREAD_ANYCOLOR);
  expandDebayerEtalon_2 = cv::imread(pathToExpandDebayer_2,
                                     cv::IMREAD_ANYDEPTH |
                                     cv::IMREAD_ANYCOLOR);
  expandDebayerEtalon_3 = cv::imread(pathToExpandDebayer_3,
                                     cv::IMREAD_ANYDEPTH |
                                     cv::IMREAD_ANYCOLOR);
  expandDebayerEtalon_4 = cv::imread(pathToExpandDebayer_4,
                                     cv::IMREAD_ANYDEPTH |
                                     cv::IMREAD_ANYCOLOR);

  expandDebayer_fastEtalon_1 = cv::imread(pathToExpandDebayer_fast_1,
                                          cv::IMREAD_ANYDEPTH |
                                          cv::IMREAD_ANYCOLOR);
  expandDebayer_fastEtalon_2 = cv::imread(pathToExpandDebayer_fast_2,
                                          cv::IMREAD_ANYDEPTH |
                                          cv::IMREAD_ANYCOLOR);
  expandDebayer_fastEtalon_3 = cv::imread(pathToExpandDebayer_fast_3,
                                          cv::IMREAD_ANYDEPTH |
                                          cv::IMREAD_ANYCOLOR);
  expandDebayer_fastEtalon_4 = cv::imread(pathToExpandDebayer_fast_4,
                                          cv::IMREAD_ANYDEPTH |
                                          cv::IMREAD_ANYCOLOR);

  expandDebayerWithoutBoundariesEtalon_1 =
      cv::imread(pathToExpandDebayerWithoutBoundaries_1,
                 cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
  expandDebayerWithoutBoundariesEtalon_2 =
      cv::imread(pathToExpandDebayerWithoutBoundaries_2,
                 cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
  expandDebayerWithoutBoundariesEtalon_3 =
      cv::imread(pathToExpandDebayerWithoutBoundaries_3,
                 cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
  expandDebayerWithoutBoundariesEtalon_4 =
      cv::imread(pathToExpandDebayerWithoutBoundaries_4,
                 cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);

  expandEasyEtalon_1 =
      cv::imread(pathToExpandEasy_1, cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
  expandEasyEtalon_2 =
      cv::imread(pathToExpandEasy_2, cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
  expandEasyEtalon_3 =
      cv::imread(pathToExpandEasy_3, cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
  expandEasyEtalon_4 =
      cv::imread(pathToExpandEasy_4, cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);

  expandLiteEtalon_1 =
      cv::imread(pathToExpandLite_1, cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
  expandLiteEtalon_2 =
      cv::imread(pathToExpandLite_2, cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
  expandLiteEtalon_3 =
      cv::imread(pathToExpandLite_3, cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
  expandLiteEtalon_4 =
      cv::imread(pathToExpandLite_4, cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);

  cv::FileStorage fileStorageAzimuth(pathToAzimuthMap, cv::FileStorage::READ);
  fileStorageAzimuth["azimuthMap"] >> azimuthMapEtalon;
  fileStorageAzimuth.release();

  cv::FileStorage fileStorageZenithMapReflectionDiffuse(
      pathToZenithMapReflectionDiffuse, cv::FileStorage::READ);
  fileStorageZenithMapReflectionDiffuse["zenithMapReflectionDiffuse"] >>
      zenithMapReflectionDiffuseEtalon;
  fileStorageZenithMapReflectionDiffuse.release();

  cv::FileStorage fileStorageZenithReflectionSpecular(
      pathToZenithMapReflectionSpecular, cv::FileStorage::READ);
  fileStorageZenithReflectionSpecular["zenithMapReflectionSpecular"] >>
      zenithMapReflectionSpecularEtalon;
  fileStorageZenithReflectionSpecular.release();

  cv::FileStorage fileStoragePolarDegree(pathToPolarDegreeMap,
                                         cv::FileStorage::READ);
  fileStoragePolarDegree["polarDegreeMap"] >> polarDegreeMapEtalon;
  fileStoragePolarDegree.release();

  LibraryParameters::configure(
      LibraryParameters::PolarizationSensorOrder::order90_45_135_0);
}

static cv::Mat epsilon(int rows, int cols) {
  // Epsilon for double values 1e-15,
  // std::numeric_limits<double>::epsilon() is not good way,
  // close values don't fit
  return cv::Mat(rows, cols, CV_64FC1, 1e-15);
}

TEST_F(ImagesTests, Read) {
  EXPECT_EQ(originalImage.type(), CV_8UC1);
}

TEST_F(ImagesTests, ExpandTest) {
  EXPECT_EQ(originalImage.type(), CV_8UC1);

  PolarExpand<uint8_t> polarExpander;
  std::vector<cv::Mat> result;

  auto start = std::chrono::high_resolution_clock::now();
  polarExpander.expand(originalImage, result);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stop - start);
  std::cout << "Time taken by function expand: " << duration.count() << "ms."
            << std::endl;

  cv::Mat zerosImage;
  cv::bitwise_xor(expandEtalon_1, result[0], zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandEtalon_2, result[1], zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandEtalon_3, result[2], zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandEtalon_4, result[3], zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);
}

TEST_F(ImagesTests, ExpandDebayerTest) {
  EXPECT_EQ(originalImage.type(), CV_8UC1);

  PolarExpand<uint8_t> polarExpander;
  std::vector<cv::Mat> result;
  auto start = std::chrono::high_resolution_clock::now();
  polarExpander.expandDebayer(originalImage, result);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stop - start);
  std::cout << "Time taken by function expandDebayer: " << duration.count()
            << "ms." << std::endl;

  cv::Mat zerosImage;
  cv::bitwise_xor(expandDebayerEtalon_1, result[0], zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandDebayerEtalon_2, result[1], zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandDebayerEtalon_3, result[2], zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandDebayerEtalon_4, result[3], zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);
}

TEST_F(ImagesTests, ExpandDebayer_fastTest) {
  EXPECT_EQ(originalImage.type(), CV_8UC1);

  PolarExpand<uint8_t> polarExpander;
  std::vector<cv::Mat> result;
  auto start = std::chrono::high_resolution_clock::now();
  polarExpander.expandDebayer_fast(originalImage, result);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stop - start);
  std::cout << "Time taken by function expandDebayer_fast: " << duration.count()
            << "ms." << std::endl;

  cv::Mat zerosImage;
  cv::bitwise_xor(expandDebayer_fastEtalon_1, result[0], zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandDebayer_fastEtalon_2, result[1], zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandDebayer_fastEtalon_3, result[2], zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandDebayer_fastEtalon_4, result[3], zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);
}

TEST_F(ImagesTests, ExpandDebayerWithoutBoundariesTest) {
  EXPECT_EQ(originalImage.type(), CV_8UC1);

  PolarExpand<uint8_t> polarExpander;
  std::vector<cv::Mat> result;
  auto start = std::chrono::high_resolution_clock::now();
  polarExpander.expandDebayerWithoutBoundaries(originalImage, result);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stop - start);
  std::cout << "Time taken by function expandDebayerWithoutBoundaries: "
            << duration.count() << "ms." << std::endl;

  cv::Mat zerosImage;
  cv::bitwise_xor(expandDebayerWithoutBoundariesEtalon_1, result[0],
                  zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandDebayerWithoutBoundariesEtalon_2, result[1],
                  zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandDebayerWithoutBoundariesEtalon_3, result[2],
                  zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandDebayerWithoutBoundariesEtalon_4, result[3],
                  zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);
}

TEST_F(ImagesTests, ExpandEasyTest) {
  EXPECT_EQ(originalImage.type(), CV_8UC1);

  PolarExpand<uint8_t> polarExpander;
  std::vector<cv::Mat> result;
  auto start = std::chrono::high_resolution_clock::now();
  polarExpander.expandEasy(originalImage, result);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stop - start);
  std::cout << "Time taken by function expandEasy: " << duration.count()
            << "ms." << std::endl;

  cv::Mat zerosImage;
  cv::bitwise_xor(expandEasyEtalon_1, result[0],
                  zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandEasyEtalon_2, result[1],
                  zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandEasyEtalon_3, result[2],
                  zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandEasyEtalon_4, result[3],
                  zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);
}

TEST_F(ImagesTests, ExpandLiteTest) {
  EXPECT_EQ(originalImage.type(), CV_8UC1);

  PolarExpand<uint8_t> polarExpander;
  std::vector<cv::Mat> result;
  auto start = std::chrono::high_resolution_clock::now();
  polarExpander.expandLite(originalImage, result);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stop - start);
  std::cout << "Time taken by function expandLite: " << duration.count()
            << "ms." << std::endl;

  cv::Mat zerosImage;
  cv::bitwise_xor(expandLiteEtalon_1, result[0],
                  zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandLiteEtalon_2, result[1],
                  zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandLiteEtalon_3, result[2],
                  zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);

  cv::bitwise_xor(expandLiteEtalon_4, result[3],
                  zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);
}

TEST_F(ImagesTests, AzimuthMapTest) {
  cv::Mat azimuthMapResult = cv::Mat(originalImage.size(), CV_64FC1,
                                     cv::Scalar(0));
  auto start = std::chrono::high_resolution_clock::now();
  computeAzimuth(expandedSource, azimuthMapResult);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stop - start);
  std::cout << "Time taken by function computeAzimuth: " << duration.count()
            << "ms." << std::endl;

  EXPECT_EQ(cv::countNonZero(cv::abs(azimuthMapResult - azimuthMapEtalon) >
                             epsilon(azimuthMapEtalon.rows,
                                     azimuthMapEtalon.cols)), 0);
}

TEST_F(ImagesTests, AzimuthMapFastTest) {
  cv::Mat azimuthMapResult = cv::Mat(originalImage.size(), CV_64FC1,
                                     cv::Scalar(0));
  auto start = std::chrono::high_resolution_clock::now();
  computeAzimuth_fast(expandedSource, azimuthMapResult);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stop - start);
  std::cout << "Time taken by function computeAzimuth_fast: "
            << duration.count() << "ms." << std::endl;

  EXPECT_EQ(cv::countNonZero(cv::abs(azimuthMapResult - azimuthMapEtalon) >
                             epsilon(azimuthMapEtalon.rows,
                                     azimuthMapEtalon.cols)), 0);
}

TEST_F(ImagesTests, PolarDegreeMapTest) {
  cv::Mat polarDegreeResult;
  auto start = std::chrono::high_resolution_clock::now();
  computePolDegree(expandedSource, polarDegreeResult);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stop - start);
  std::cout << "Time taken by function computePolDegree: " << duration.count()
            << "ms." << std::endl;

  EXPECT_EQ(cv::countNonZero(cv::abs(polarDegreeResult - polarDegreeMapEtalon) >
                             epsilon(polarDegreeMapEtalon.rows,
                                     polarDegreeMapEtalon.cols)), 0);
}

TEST_F(ImagesTests, ZenithMapReflectionDiffuseTest) {
  cv::Mat zenithMapResult = cv::Mat(originalImage.size(), CV_64FC1,
                                    cv::Scalar(0));

  auto start = std::chrono::high_resolution_clock::now();
  zenithMapResult = zenith_eval(ReflectionDiffuse, polarDegreeMapEtalon,
                                zenithN, zenithPointsNumber);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stop - start);
  std::cout << "Time taken by function zenith_eval (ReflectionDiffuse): "
            << duration.count() << "ms." << std::endl;

  EXPECT_EQ(cv::countNonZero(
      cv::abs(zenithMapResult - zenithMapReflectionDiffuseEtalon) >
      epsilon(zenithMapReflectionDiffuseEtalon.rows,
              zenithMapReflectionDiffuseEtalon.cols)), 0);
}

TEST_F(ImagesTests, ZenithMapReflectionDiffuseFastTest) {
  cv::Mat zenithMapResult = cv::Mat(originalImage.size(), CV_64FC1,
                                    cv::Scalar(0));

  auto start = std::chrono::high_resolution_clock::now();
  zenithMapResult = zenith_eval_diffuse_fast(polarDegreeMapEtalon, zenithN);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stop - start);
  std::cout << "Time taken by function zenith_eval_fast (ReflectionDiffuse): "
            << duration.count() << "ms." << std::endl;

  EXPECT_EQ(cv::countNonZero(
      cv::abs(zenithMapResult - zenithMapReflectionDiffuseEtalon) >
      epsilon(zenithMapReflectionDiffuseEtalon.rows,
              zenithMapReflectionDiffuseEtalon.cols)), 0);
}

TEST_F(ImagesTests, ZenithMapReflectionSpecularTest) {
  cv::Mat zenithMapResult = cv::Mat(originalImage.size(), CV_64FC1,
                                    cv::Scalar(0));

  auto start = std::chrono::high_resolution_clock::now();
  zenithMapResult = zenith_eval(ReflectionSpecular, polarDegreeMapEtalon,
                                zenithN, zenithPointsNumber);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      stop - start);
  std::cout << "Time taken by function zenith_eval (ReflectionSpecular): "
            << duration.count() << "ms." << std::endl;

  cv::Mat layersEtalon[2];
  split(zenithMapReflectionSpecularEtalon, layersEtalon);

  cv::Mat layersResult[2];
  split(zenithMapResult, layersResult);

  auto difference_map0 =
      cv::abs(layersResult[0] - layersEtalon[0]);
  auto difference_map1 =
      cv::abs(layersResult[1] - layersEtalon[1]);

  EXPECT_EQ(
      cv::countNonZero(
          difference_map0 > epsilon(zenithMapReflectionSpecularEtalon.rows,
                                    zenithMapReflectionSpecularEtalon.cols)),
      0);

  EXPECT_EQ(
      cv::countNonZero(
          difference_map1 > epsilon(zenithMapReflectionSpecularEtalon.rows,
                                    zenithMapReflectionSpecularEtalon.cols)),
      0);
}
