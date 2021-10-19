//------------------------------------------------------------------------------
//  Created     : 06.04.2021
//  Author      : Golovchanskaya Julia
//  Description : Tests for algorithm "Edge Aware" and secondary functions.
//------------------------------------------------------------------------------

#include "AzimuthEvaluation.h"
#include "CommonFunctions.h"
#include "EdgeAware.h"
#include "LibraryParameters.h"
#include "PolarExpand.h"
#include "ZenithEvaluation.h"

#include <gtest/gtest.h>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <filesystem>
#include <fstream>

void clearString(std::string &str) {
  if (*str.begin() == '[') {
    str = str.substr(1);
  }
  if (*str.end() == ']') {
    str = str.substr(0, str.size() - 1);
  }
}

std::pair<double, double>
getLineFromPoints(cv::Point2d point0, cv::Point2d point1) {
  double dy = point0.y - point1.y;
  double dx = point0.x - point1.x;

  if (abs(dx) < 1e-5) {
    dx = 1e-5;
  }

  double slope = dy / dx;
  double bias = point0.y - slope * point0.x;

  return {slope, bias};
}

void putPointsOnLine(cv::Point2d point0, cv::Point2d point1, int parts,
                     std::vector<cv::Point2d> &result) {
  auto slopeAndBias = getLineFromPoints(point0, point1);

  double dx = point1.x - point0.x;
  double step = dx / parts;
  for (int i = 1; i < parts; ++i) {
    double x0 = point0.x + step * i;
    result.emplace_back(
    cv::Point2d(x0, slopeAndBias.first * x0 + slopeAndBias.second));
  }
}

bool isRotationMatrix(cv::Mat &R) {
  cv::Mat Rt;
  transpose(R, Rt);
  cv::Mat shouldBeIdentity = Rt * R;
  cv::Mat I = cv::Mat::eye(3, 3, shouldBeIdentity.type());

  return norm(I, shouldBeIdentity) < 1e-6;
}

// Matlab style
cv::Vec3d rotm2eulZYZ(cv::Mat &R) {
  assert(isRotationMatrix(R));
  double psi = atan2(R.at<double>(1, 2),
                     -R.at<double>(0, 2));
  double phi = atan2(R.at<double>(2, 1),
                     R.at<double>(2, 0));
  double theta = atan2(R.at<double>(2, 0) * cos(phi) +
                       R.at<double>(2, 1) * sin(phi),
                       R.at<double>(2, 2));
//  "-" in matlab version. Original code is here
//  https://math.stackexchange.com/questions/3328656/convert-rotation-matrix-to-euler-angles-zyz-y-convention-analytically
  //TODO: Why does it have minuses?
  return cv::Vec3f(-psi, -theta, -phi);
}

std::vector<double> getVectorFromString(const std::string &str) {
  std::stringstream ss(str);
  std::string item;
  std::vector<double> result_vector;
  while (std::getline(ss, item, ',')) {
    clearString(item);
    result_vector.emplace_back(std::stod(item));
  }

  return result_vector;
}

void
loadCameraParameters(std::string pathToCameraParameters, cv::Mat &cameraMatrix,
                     cv::Mat &distortionCoeffs, std::vector<cv::Mat> &rvecs,
                     std::vector<cv::Mat> &tvecs) {
  std::ifstream cameraParametersFile;
  cameraParametersFile.open(pathToCameraParameters);

  std::vector<std::string> lines;
  std::string line;
  while (std::getline(cameraParametersFile, line)) {
    lines.emplace_back(line);
  }
  cameraParametersFile.close();

  auto cameraMatrixPtr = std::find(lines.begin(), lines.end(), "K");
  auto distCoeffsPtr = std::find(lines.begin(), lines.end(), "D");
  auto rvecsPtr = std::find(lines.begin(), lines.end(), "Rvecs");
  auto tvecsPtr = std::find(lines.begin(), lines.end(), "Tvecs");

  // camera matrix parsing
  cameraMatrix = cv::Mat::zeros(cv::Size(3, 3), CV_64FC1);
  if (cameraMatrixPtr != lines.end()) {
    std::vector<double> cameraMatrixVector = getVectorFromString(
    cameraMatrixPtr[1]);
    for (int i = 0; i < cameraMatrix.rows; ++i) {
      for (int j = 0; j < cameraMatrix.cols; ++j) {
        cameraMatrix.at<double>(i, j) = cameraMatrixVector[
        i * cameraMatrix.cols + j];
      }
    }
  }

  // distortion
  distortionCoeffs = cv::Mat::zeros(cv::Size(5, 1), CV_64FC1);
  if (distCoeffsPtr != lines.end()) {
    std::vector<double> distCoeffsVector = getVectorFromString(
    distCoeffsPtr[1]);
    for (int i = 0; i < distortionCoeffs.cols; ++i) {
      distortionCoeffs.at<double>(i) = distCoeffsVector[i];
    }
  }

  if (rvecsPtr != lines.end()) {
    auto ptr = rvecsPtr + 1;
    while (ptr != tvecsPtr) {
      std::vector<double> rvecVector = getVectorFromString(*ptr);
      rvecs.emplace_back(cv::Mat::zeros(cv::Size(1, 3), CV_64FC1));
      auto last_rvec = rvecs.end() - 1;
      for (int i = 0; i < last_rvec->rows; ++i) {
        last_rvec->at<double>(i) = rvecVector[i];
      }
      ++ptr;
    }
  }

  if (tvecsPtr != lines.end()) {
    auto ptr = tvecsPtr + 1;
    while (ptr != lines.end()) {
      std::vector<double> tvecVector = getVectorFromString(*ptr);
      tvecs.emplace_back(cv::Mat::zeros(cv::Size(1, 3), CV_64FC1));
      auto last_rvec = tvecs.end() - 1;
      for (int i = 0; i < last_rvec->rows; ++i) {
        last_rvec->at<double>(i) = tvecVector[i];
      }
      ++ptr;
    }
  }
}

void expandWithEdgeAware(cv::Mat image, std::vector<cv::Mat> &polarSet) {
  std::vector<cv::Mat> expandedSourcesBilinearly;
  std::vector<cv::Mat> expandedSourcesEasy;

  PolarExpand<uint8_t> polarExpander;
  polarExpander.expand(image, expandedSourcesBilinearly);
  polarExpander.expandEasy(image, expandedSourcesEasy);

  image.convertTo(image, CV_64FC1);

  std::vector<cv::Mat> edges = getPreparedEdges(expandedSourcesEasy);

  polarSet = firstStageEdgesEstimation(
  image, edges, expandedSourcesBilinearly);
  secondStageEdgesEstimation(edges, polarSet);

  for (auto &polarImage : polarSet) {
    polarImage.convertTo(polarImage, CV_8UC1);
  }
}

class EdgeAwareTests : public ::testing::Test {
public:
  EdgeAwareTests();

public:
  static constexpr double testEpsilon = 1e-10;

protected:
  // Paths
  const std::string _testDataFolder = TEST_DATA_FOLDER;
  std::string pathToOriginalImage = _testDataFolder + "/chessboardSPA0.tiff";
  std::string pathToMatlabResizedExpandEasyCanny0 =
      _testDataFolder + "/chessboardSPA0MatlabResizedExpandEasyCanny0.png";
  std::string pathToMatlabExtendedEdgesEtalon0 =
      _testDataFolder + "/chessboardSPA0MatlabExtendedEdgesEtalon0.png";

  std::vector<std::string> pathToMatlabFirstStageEtalons = {
      _testDataFolder + "/chessboardSPA0FirstStageEtalon0.png",
      _testDataFolder + "/chessboardSPA0FirstStageEtalon1.png",
      _testDataFolder + "/chessboardSPA0FirstStageEtalon2.png",
      _testDataFolder + "/chessboardSPA0FirstStageEtalon3.png"};

  std::vector<std::string> pathToMatlabSecondStageEtalons = {
      _testDataFolder + "/chessboardSPA0SecondStageEtalon0.png",
      _testDataFolder + "/chessboardSPA0SecondStageEtalon1.png",
      _testDataFolder + "/chessboardSPA0SecondStageEtalon2.png",
      _testDataFolder + "/chessboardSPA0SecondStageEtalon3.png"};

  cv::Mat originalImage;
  std::vector<cv::Mat> expandedSources;

  cv::Mat matlabResizedExpandEasyCanny0;
  cv::Mat matlabExtendedEdgesEtalon0;

  std::vector<cv::Mat> matlabFirstStageEtalons;
  std::vector<cv::Mat> matlabSecondStageEtalons;

  std::vector<cv::Mat> edgesResult;


  cv::Mat pixelCovariance1Result;
  cv::Mat pixelCovariance2Result;
  int rowTestIndex = 737;
  int colTestIndex = 931;
  std::vector<double> pixelCovariance1Values = {0.7723832702423125,
                                                -0.4166711406644622,
                                                0.9568763776718594,
                                                -0.317788353221411};
  std::vector<double> pixelCovariance2Values = {0.506715502275236,
                                                -0.05058827682758858,
                                                0.6078326859618125,
                                                -0.06790352612814976};

  double covEstimateFalseModeEtalon = 228.3927708476905138;
  double covEstimateTrueModeEtalon = 236.9743780833439359;
  double blEstimate1Etalon = 230.0;
  double blEstimate2Etalon = 240.0;

  // Path
  std::string pathToDataset = "/home/golju/Data/TMK/2019_02_18_lucid/data_mono8/"; // FIXME
  std::string pathToCameraParameters =
  pathToDataset + "/cameraParams.txt"; // FIXME
  // it is special for this dataset
  const size_t boardHeight = 8;
  const size_t boardWidth = 7;
  const size_t squareSize = 50;

  cv::Mat cameraMatrix;
  cv::Mat distortionCoeffs;
  std::vector<cv::Mat> rvecs, tvecs;

  const float zenithN = 1.3;
  const int zenithPointsNumber = 2000;

  std::vector<std::string> filePaths;

  void evaluateTest(const std::string &methodName,
                    std::function<void(cv::Mat,
                                       std::vector<cv::Mat> &)> &expandMethod);
};

EdgeAwareTests::EdgeAwareTests() {
  originalImage = cv::imread(pathToOriginalImage,
                             cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
  PolarExpand<uint8_t> polarExpander;
  polarExpander.expand(originalImage, expandedSources);

  originalImage.convertTo(originalImage, CV_64FC1);

  matlabResizedExpandEasyCanny0 = cv::imread(
  pathToMatlabResizedExpandEasyCanny0,
  cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
  matlabExtendedEdgesEtalon0 = cv::imread(pathToMatlabExtendedEdgesEtalon0,
                                          cv::IMREAD_ANYDEPTH |
                                          cv::IMREAD_ANYCOLOR);

  matlabFirstStageEtalons.resize(pathToMatlabFirstStageEtalons.size());
  matlabSecondStageEtalons.resize(pathToMatlabSecondStageEtalons.size());

  for (size_t i = 0; i < matlabFirstStageEtalons.size(); ++i) {
    matlabFirstStageEtalons[i] =
        cv::imread(pathToMatlabFirstStageEtalons[i],
                   cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
    matlabSecondStageEtalons[i] =
        cv::imread(pathToMatlabSecondStageEtalons[i],
                   cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
  }

  edgesResult.resize(4);
  for (size_t i = 0; i < edgesResult.size(); ++i) {
    edgesResult[i] =
        getExtendedEdges(getFilteredCannyImage(expandedSources[i]), 3);
  }

  pixelCovariance1Result =
      cv::Mat(cv::Size(1, 4), CV_64FC1, pixelCovariance1Values.data());
  pixelCovariance2Result =
      cv::Mat(cv::Size(1, 4), CV_64FC1, pixelCovariance2Values.data());

  loadCameraParameters(pathToCameraParameters, cameraMatrix, distortionCoeffs,
                       rvecs, tvecs);

  LibraryParameters::configure(
      LibraryParameters::PolarizationSensorOrder::order90_45_135_0);

  filePaths = getFilePaths(pathToDataset + "*.tiff");
  std::cout << "Dataset: " << pathToDataset << std::endl;
  std::cout << "Number of files: " << filePaths.size() << std::endl;
}

void EdgeAwareTests::evaluateTest(const std::string &methodName,
                                  std::function<void(cv::Mat,
                                                     std::vector<cv::Mat> &)> &expandMethod) {
  std::vector<cv::Point3f> objectPoints = createChessboardPoints(boardHeight,
                                                                 boardWidth,
                                                                 squareSize);
  std::ofstream mediansFile;
  mediansFile.open(pathToDataset + "/" + methodName + "Medians.txt");

  std::ofstream stdsFile;
  stdsFile.open(pathToDataset + "/" + methodName + "Stds.txt");

  for (size_t image_num = 0; image_num < filePaths.size(); ++image_num) {
    std::vector<cv::Point2f> imagePoints;
    cv::projectPoints(objectPoints, rvecs[image_num], tvecs[image_num],
                      cameraMatrix, distortionCoeffs, imagePoints);
    std::reverse(imagePoints.begin(), imagePoints.end()); // FIXME:

    // Preparing images
    cv::Mat imageOriginal = cv::imread(
        filePaths[image_num], cv::IMREAD_ANYDEPTH | cv::IMREAD_ANYCOLOR);
    cv::Mat undistortedImage;
    cv::undistort(imageOriginal, undistortedImage, cameraMatrix,
                  distortionCoeffs);

    // Maps calculation
    std::vector<cv::Mat> expandedSource;
    expandMethod(undistortedImage, expandedSource);
    cv::Mat azimuthMap =
        cv::Mat(undistortedImage.size(), CV_64FC1, cv::Scalar(0));
    computeAzimuth(expandedSource, azimuthMap);
    cv::Mat zenithMap =
        cv::Mat(undistortedImage.size(), CV_64FC1, cv::Scalar(0));
    cv::Mat polarDegreeMap;
    computePolDegree(expandedSource, polarDegreeMap);
    zenithMap = zenith_eval(ReflectionDiffuse, polarDegreeMap, zenithN,
                            zenithPointsNumber);

    cv::imwrite("/tmp/maps/" + methodName + "azim.png", azimuthMap);
    cv::imwrite("/tmp/maps/" + methodName + "zenit.png", zenithMap);

    std::vector<double> azimuthAnglesMedians;
    std::vector<double> azimuthAnglesStds;

    std::vector<double> zenithAnglesMedians;
    std::vector<double> zenithAnglesStds;

    int numberOfParts = 30;
    for (size_t i = 0; i < boardHeight - 1; ++i) {
      for (size_t j = 0; j < boardWidth - 1; ++j) {
        std::vector<cv::Point2d> allPointsPerCell;
        putPointsOnLine(imagePoints[i * boardWidth + j],
                        imagePoints[i * boardWidth + (j + 1)],
                        numberOfParts, allPointsPerCell);
        putPointsOnLine(imagePoints[i * boardWidth + j],
                        imagePoints[(i + 1) * boardWidth + j],
                        numberOfParts, allPointsPerCell);
        putPointsOnLine(imagePoints[(i + 1) * boardWidth + j],
                        imagePoints[(i + 1) * boardWidth + (j + 1)],
                        numberOfParts, allPointsPerCell);
        putPointsOnLine(imagePoints[i * boardWidth + (j + 1)],
                        imagePoints[(i + 1) * boardWidth + (j + 1)],
                        numberOfParts, allPointsPerCell);

        allPointsPerCell.emplace_back(imagePoints[i * boardWidth + j]);
        allPointsPerCell.emplace_back(imagePoints[i * boardWidth + (j + 1)]);
        allPointsPerCell.emplace_back(imagePoints[(i + 1) * boardWidth + j]);
        allPointsPerCell.emplace_back(
        imagePoints[(i + 1) * boardWidth + (j + 1)]);

        std::vector<double> azimuthAngles;
        azimuthAngles.reserve(allPointsPerCell.size());
        std::vector<double> zenithAngles;
        zenithAngles.reserve(allPointsPerCell.size());
        for (auto &point : allPointsPerCell) {
          azimuthAngles.emplace_back(azimuthMap.at<double>(point));
          zenithAngles.emplace_back(zenithMap.at<double>(point));
        }
        azimuthAnglesMedians.emplace_back(getMedian(azimuthAngles));
        azimuthAnglesStds.emplace_back(getStd(azimuthAngles));

        zenithAnglesMedians.emplace_back(getMedian(zenithAngles));
        zenithAnglesStds.emplace_back(getStd(zenithAngles));
      }
    }

    // Rodrigues vector to Euler vector ZYZ
    cv::Mat rotationMatrix;
    cv::Rodrigues(rvecs[image_num], rotationMatrix);
    cv::Vec3d eulerAnglesZYZ = rotm2eulZYZ(rotationMatrix);
    double thetaAngleEtalon = eulerAnglesZYZ(1); // Azimuth
    double phiAngleEtalon = eulerAnglesZYZ(0); // Zenith

    // Save to file medians
    mediansFile
    << std::filesystem::path(filePaths[image_num]).filename().string()
    << "\n";
    mediansFile << "azimuth_error\n";
    for (size_t i = 0; i < azimuthAnglesMedians.size(); ++i) {
      mediansFile << abs(azimuthAnglesMedians[i] - thetaAngleEtalon)
                  << " ";
    }
    mediansFile << "\n";
    mediansFile << "zenith_error" << std::endl;
    for (size_t i = 0; i < zenithAnglesMedians.size(); ++i) {
      mediansFile << abs(zenithAnglesMedians[i] - phiAngleEtalon)
                  << " ";
    }
    mediansFile << "\n";

    // Save to file stds
    stdsFile
    << std::filesystem::path(filePaths[image_num]).filename().string()
    << "\n";
    stdsFile << "azimuth_error\n";
    for (size_t i = 0; i < azimuthAnglesStds.size(); ++i) {
      stdsFile << azimuthAnglesStds[i] << " ";
    }
    stdsFile << "\n";
    stdsFile << "zenith_error" << std::endl;
    for (size_t i = 0; i < zenithAnglesStds.size(); ++i) {
      stdsFile << zenithAnglesStds[i] << " ";
    }
    stdsFile << "\n";
  }

  mediansFile.close();
  stdsFile.close();
}

static cv::Mat epsilon(int rows, int cols) {
  return cv::Mat(rows, cols, CV_64FC1, EdgeAwareTests::testEpsilon);
}

TEST_F(EdgeAwareTests, ExtendedEdgesTest) {
  cv::Mat extendedEdgesResult0 = getExtendedEdges(matlabResizedExpandEasyCanny0,
                                                  3);
  cv::Mat zerosImage;
  cv::bitwise_xor(extendedEdgesResult0, matlabExtendedEdgesEtalon0, zerosImage);
  EXPECT_EQ(cv::countNonZero(zerosImage), 0);
}

TEST_F(EdgeAwareTests, PixCovariance1Test) {
  cv::Mat result = PixCovariance1(originalImage, rowTestIndex, colTestIndex,
                                  6);

  EXPECT_EQ(cv::countNonZero(cv::abs(result - pixelCovariance1Result) >
                             epsilon(pixelCovariance1Result.rows,
                                     pixelCovariance1Result.cols)), 0);
}

TEST_F(EdgeAwareTests, PixCovariance2Test) {
  cv::Mat result = PixCovariance2(originalImage, rowTestIndex, colTestIndex,
                                  6);

  EXPECT_EQ(cv::countNonZero(cv::abs(result - pixelCovariance2Result) >
                             epsilon(pixelCovariance2Result.rows,
                                     pixelCovariance2Result.cols)), 0);
}

TEST_F(EdgeAwareTests, CovEstimateTest) {
  double result = CovEstimate(originalImage, rowTestIndex, colTestIndex,
                              false);
  EXPECT_LE(abs(result - covEstimateFalseModeEtalon),
            EdgeAwareTests::testEpsilon);
  result = CovEstimate(originalImage, rowTestIndex, colTestIndex,
                       true);
  EXPECT_LE(abs(result - covEstimateTrueModeEtalon),
            EdgeAwareTests::testEpsilon);
}

TEST_F(EdgeAwareTests, BlEstimate1Test) {
  double result = BlEstimate1(originalImage, rowTestIndex, colTestIndex);
  EXPECT_LE(abs(result - blEstimate1Etalon),
            EdgeAwareTests::testEpsilon);
}

TEST_F(EdgeAwareTests, BlEstimate2Test) {
  double result = BlEstimate2(originalImage, rowTestIndex, colTestIndex);
  EXPECT_LE(abs(result - blEstimate2Etalon),
            EdgeAwareTests::testEpsilon);
}

TEST_F(EdgeAwareTests, FirstStageEdgesEstimationTest) {
  cv::Mat originalImageUint8;
  originalImage.convertTo(originalImageUint8, CV_64FC1);
  std::vector<cv::Mat> firstStageResult = firstStageEdgesEstimation(
  originalImageUint8, edgesResult,
  expandedSources);

  std::vector<cv::Mat> firstStageResultUint8(firstStageResult.size());
  for (size_t i = 0; i < firstStageResult.size(); ++i) {
    firstStageResult[i].convertTo(firstStageResultUint8[i], CV_8UC1);
  }

  double minVal;
  double maxVal;
  cv::Point minLoc;
  cv::Point maxLoc;
  cv::Mat difference;

  for (size_t i = 0; i < matlabFirstStageEtalons.size(); ++i) {
    difference = cv::abs(firstStageResultUint8[i] - matlabFirstStageEtalons[i]);
    minMaxLoc(difference, &minVal, &maxVal, &minLoc, &maxLoc);
    EXPECT_LE(maxVal, 1);
  }
}

TEST_F(EdgeAwareTests, SecondStageEdgesEstimationTest) {
  std::vector<cv::Mat> secondStageResult(matlabFirstStageEtalons.size());
  for (size_t i = 0; i < secondStageResult.size(); ++i) {
    matlabFirstStageEtalons[i].convertTo(secondStageResult[i], CV_64FC1);
  }

  secondStageEdgesEstimation(edgesResult, secondStageResult);

  std::vector<cv::Mat> secondStageResultUint8(secondStageResult.size());
  for (size_t i = 0; i < secondStageResultUint8.size(); ++i) {
    secondStageResult[i].convertTo(secondStageResultUint8[i], CV_8UC1);
  }

  double minVal;
  double maxVal;
  cv::Point minLoc;
  cv::Point maxLoc;
  cv::Mat difference;

  for (size_t i = 0; i < matlabSecondStageEtalons.size(); ++i) {
    difference = cv::abs(
    secondStageResultUint8[i] - matlabSecondStageEtalons[i]);
    minMaxLoc(difference, &minVal, &maxVal, &minLoc, &maxLoc);
    EXPECT_LE(maxVal, 1);
  }
}

TEST_F(EdgeAwareTests, DISABLED_ExpandEasyTest) {
  std::string methodName = "expandEasy";

  // Method wrapper
  PolarExpand<uint8_t> polarExpander;
  std::function<void(cv::Mat, std::vector<cv::Mat> &)> expandEasy(
  [&polarExpander](cv::Mat image, std::vector<cv::Mat> &polarSet) {
    polarExpander.expandEasy(image, polarSet);
    for (auto &polarImage : polarSet) {
      cv::resize(polarImage, polarImage, cv::Size(), 2, 2);
    }
  });
  evaluateTest(methodName, expandEasy);
}

TEST_F(EdgeAwareTests, DISABLED_ExpandDebayer_fastTest) {
  std::string methodName = "expandDebayer_fast";

  // Method wrapper
  PolarExpand<uint8_t> polarExpander;
  std::function<void(cv::Mat, std::vector<cv::Mat> &)> expandDebayer_fast(
  [&polarExpander](cv::Mat image, std::vector<cv::Mat> &polarSet) {
    polarExpander.expandDebayer_fast(image, polarSet);
  });
  evaluateTest(methodName, expandDebayer_fast);
}

TEST_F(EdgeAwareTests, DISABLED_ExpandTest) {
  std::string methodName = "expand";

  // Method wrapper
  PolarExpand<uint8_t> polarExpander;
  std::function<void(cv::Mat, std::vector<cv::Mat> &)> expand(
  [&polarExpander](cv::Mat image, std::vector<cv::Mat> &polarSet) {
    polarExpander.expand(image, polarSet);
  });
  evaluateTest(methodName, expand);
}

TEST_F(EdgeAwareTests, DISABLED_ExpandDebayerTest) {
  std::string methodName = "expandDebayer";

  // Method wrapper
  PolarExpand<uint8_t> polarExpander;
  std::function<void(cv::Mat, std::vector<cv::Mat> &)> expandDebayer(
  [&polarExpander](cv::Mat image, std::vector<cv::Mat> &polarSet) {
    polarExpander.expandDebayer(image, polarSet);
  });
  evaluateTest(methodName, expandDebayer);
}

TEST_F(EdgeAwareTests, DISABLED_ExpandDebayerWithoutBoundariesTest) {
  std::string methodName = "expandDebayerWithoutBoundaries";

  // Method wrapper
  PolarExpand<uint8_t> polarExpander;
  std::function<void(cv::Mat,
                     std::vector<cv::Mat> &)> expandDebayerWithoutBoundaries(
  [&polarExpander](cv::Mat image, std::vector<cv::Mat> &polarSet) {
    polarExpander.expandDebayerWithoutBoundaries(image, polarSet);
  });
  evaluateTest(methodName, expandDebayerWithoutBoundaries);
}

TEST_F(EdgeAwareTests, DISABLED_ExpandWithEdgeAwareTest) {
  std::string methodName = "expandWithEdgeAware";

  // Method wrapper
  PolarExpand<uint8_t> polarExpander;
  std::function<void(cv::Mat,
                     std::vector<cv::Mat> &)> expandWithEdgeAwareL(
  [](cv::Mat image, std::vector<cv::Mat> &polarSet) {
    expandWithEdgeAware(image, polarSet);
  });
  evaluateTest(methodName, expandWithEdgeAwareL);
}