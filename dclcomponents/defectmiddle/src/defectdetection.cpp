#include "defectdetection.h"

#include <torch/data.h> // transforms::Normalize
#include <torch/script.h> // ::Module

#include <filesystem>


//==============================================================================
//========================== Free functions ====================================
//==============================================================================
static at::Tensor toFloatTensor(const cv::Mat &img)
{
  at::Tensor tensor_image = torch::from_blob(img.data, {img.rows, img.cols, 3 },
                                             at::kByte).clone();
  tensor_image.unsqueeze_(0);
  tensor_image = tensor_image.to(at::kFloat);
  return tensor_image.permute({0, 3, 1, 2});
}

static std::vector<torch::jit::IValue> toInput(const at::Tensor &tensor_image)
{
  return std::vector<torch::jit::IValue>{tensor_image};
}

static std::vector<torch::jit::IValue>
toInput(const std::vector<at::Tensor> &tensors)
{
  std::vector<torch::jit::IValue> result;
  for (auto &tensor : tensors) {
    result.emplace_back(tensor);
  }
  return result;
}

//==============================================================================
//========================== DefectDetectionNetworkV1 ==========================
//==============================================================================
namespace DDNV1Variables
{
  static torch::jit::script::Module module;
  static constexpr double means[3] = {255 * 0.485, 255 * 0.456, 255 * 0.406};
  static constexpr double stddevs[3] = {255 * 0.229, 255 * 0.224, 255 * 0.225};
  static constexpr double thresholds[4] = {0.5, 0.5, 0.5, 0.5};
  static constexpr int inputWidth = 1600;
  static constexpr int inputHeight = 256;
  static constexpr double inputRatio = double(inputHeight) / inputWidth;
};

cv::Mat DefectDetectionNetworkV1::getDefectMask(const cv::Mat &image)
{
  double scaleToAnswer = 0.0;
  cv::Mat input = prepareForInfer(image, scaleToAnswer);
  cv::Mat inferResult = infer(input);
  cv::resize(inferResult, inferResult, cv::Size(), scaleToAnswer,
             scaleToAnswer, cv::INTER_LINEAR);
  cv::Mat result = cv::Mat::zeros(image.size(), CV_8UC1);

  cv::Rect roi((result.cols - inferResult.cols) >> 1,
               (result.rows - inferResult.rows) >> 1,
               inferResult.cols, inferResult.rows);

  inferResult.copyTo(result(roi));
  return result;
}

int DefectDetectionNetworkV1::load(const std::string &pathToModule)
{
  torch::NoGradGuard noGrad;
  int errorCode = 0;
  if (std::filesystem::exists(pathToModule)) {
    try {
      DDNV1Variables::module = torch::jit::load(pathToModule);
    } catch (const c10::Error& e) {
      fprintf(stderr, "DefectDetectionNetworkV1: there is something "
                      "wrong with checkpoints!\n");
      errorCode = 1;
    }
  } else {
    fprintf(stderr, "DefectDetectionNetworkV1: No checkpoints file!\n");
    errorCode = 2;
  }
  return errorCode;
}

cv::Mat DefectDetectionNetworkV1::infer(const cv::Mat &image)
{
  at::Tensor tensorImage = toFloatTensor(image);
  tensorImage =
          torch::data::transforms::Normalize<>(DDNV1Variables::means,
                  DDNV1Variables::stddevs)(tensorImage);

  auto input = toInput(tensorImage);
  at::Tensor output = DDNV1Variables::module.forward(input).toTensor();

  int width = output.size(3);
  int height = output.size(2);
  int layerSize = width * height;
  auto dataLayer0 = output.data_ptr<float_t>();
  auto dataLayer1 = dataLayer0 + layerSize;
  auto dataLayer2 = dataLayer1 + layerSize;
  auto dataLayer3 = dataLayer2 + layerSize;

  cv::Mat answer = cv::Mat::zeros(cv::Size{ width, height }, CV_8U);
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      if (dataLayer0[j] > DDNV1Variables::thresholds[0] ||
          dataLayer1[j] > DDNV1Variables::thresholds[1] ||
          dataLayer2[j] > DDNV1Variables::thresholds[2] ||
          dataLayer3[j] > DDNV1Variables::thresholds[3]) {
        answer.at<uchar>(i, j) = 255;
      }
    }
    dataLayer0 += width;
    dataLayer1 += width;
    dataLayer2 += width;
    dataLayer3 += width;
  }
  return answer;
}

cv::Mat DefectDetectionNetworkV1::prepareForInfer(const cv::Mat &image,
                                                  double &scaleToAnswer)
{
  cv::Mat result;
  std::vector<cv::Mat> channels(3);

  if (image.type() == CV_8UC1) {
    channels = {image, image, image};
  } else if (image.type() == CV_8UC3) {
    cv::Mat imageGray;
    cv::cvtColor(image, imageGray, cv::COLOR_BGR2GRAY); // Attention! It is assumed that image is BGR!
    channels = {imageGray, imageGray, imageGray};
  } else if (image.type() == CV_16UC1) {
    cv::Mat imageMono8;
    constexpr double alpha = 1.0/256;
    image.convertTo(imageMono8, CV_8U, alpha);
    channels = {imageMono8, imageMono8, imageMono8};
  } else {
    fprintf(stderr, "DefectDetectionNetworkV1: doesn't "
                    "know this image format!\n");
    cv::Mat zeros = cv::Mat::zeros(image.size(), CV_8UC1);
    channels = {zeros, zeros, zeros};
  }
  // FIXME: there will be more types
  cv::merge(channels, result);

  int tempWidth = image.cols;
  int tempHeight = image.cols * DDNV1Variables::inputRatio;
  if (tempHeight > image.rows) {
    tempHeight = image.rows;
    tempWidth = image.rows / DDNV1Variables::inputRatio;
  }
  scaleToAnswer = double(tempWidth) / DDNV1Variables::inputWidth;

  cv::Rect roi((image.cols - tempWidth) >> 1, (image.rows - tempHeight) >> 1,
               tempWidth, tempHeight);
  result = result(roi);
  cv::resize(result, result, cv::Size(DDNV1Variables::inputWidth,
          DDNV1Variables::inputHeight), 0, 0, cv::INTER_LINEAR);
  return result;
}

//==============================================================================
//========================== DefectDetectionNetworkV2 ==========================
//==============================================================================

namespace DDNV2Variables
{
  static torch::jit::script::Module encoder;
  static torch::jit::script::Module decoder;
  static constexpr double means[3] = {255 * 0.485, 255 * 0.456, 255 * 0.406};
  static constexpr double stddevs[3] = {255 * 0.229, 255 * 0.224, 255 * 0.225};
  static constexpr double thresholds[4] = {0.5, 0.5, 0.5, 0.5};
  static constexpr int inputWidth = 1600;
  static constexpr int inputHeight = 256;
  static constexpr double inputRatio = double(inputHeight) / inputWidth;
}

int DefectDetectionNetworkV2::load(const std::string &pathToFolder)
{
  torch::NoGradGuard noGrad;
  int errorCode = 0;
  if (std::filesystem::exists(pathToFolder + "/decoder.pth") &&
      std::filesystem::exists(pathToFolder + "/encoder.pth") ) {
    try {
      DDNV2Variables::encoder = torch::jit::load(pathToFolder + "/encoder.pth");
    } catch (const c10::Error& e) {
      fprintf(stderr, "DefectDetectionNetworkV2: there is something "
                      "wrong with checkpoints (encoder.pth)!\n");
      errorCode = 1;
    }
    try {
      DDNV2Variables::decoder = torch::jit::load(pathToFolder + "/decoder.pth");
    } catch (const c10::Error& e) {
      fprintf(stderr, "DefectDetectionNetworkV2: there is something "
                      "wrong with checkpoints (decoder.pth)!\n");
      errorCode = 1;
    }
  } else {
    fprintf(stderr, "DefectDetectionNetworkV2: No checkpoints files!\n");
    errorCode = 2;
  }
  return errorCode;
}

cv::Mat DefectDetectionNetworkV2::getDefectMask(const cv::Mat &image)
{
  double scaleToAnswer = 0.0;
  cv::Mat input = prepareForInfer(image, scaleToAnswer);
  cv::Mat inferResult = infer(input);
  cv::resize(inferResult, inferResult, cv::Size(), scaleToAnswer,
             scaleToAnswer, cv::INTER_LINEAR);
  cv::Mat result = cv::Mat::zeros(image.size(), CV_8UC1);

  cv::Rect roi((result.cols - inferResult.cols) >> 1,
               (result.rows - inferResult.rows) >> 1,
               inferResult.cols, inferResult.rows);

  inferResult.copyTo(result(roi));
  return result;
}

cv::Mat DefectDetectionNetworkV2::infer(const cv::Mat &image)
{
  at::Tensor tensorImage = toFloatTensor(image);
  tensorImage = torch::data::transforms::Normalize<>(DDNV2Variables::means,
          DDNV2Variables::stddevs)(tensorImage);

  auto inputEncoder = toInput(tensorImage);
  auto outputEncoder =
          DDNV2Variables::encoder.forward(inputEncoder).toTuple()->elements();

  int labelsSize = 4; // It depends on neural network
  auto labels = outputEncoder.at(0).toTensor().data_ptr<float_t>();

  auto features = outputEncoder.at(1).toTensorVector();
  bool isAnyDeffectsFound = false;
  at::Tensor outputDecoder;
  for (int i = 0; i < labelsSize; ++i) {
    if (labels[i] > 0) {
      outputDecoder =
              DDNV2Variables::decoder.forward(toInput(features)).toTensor();
      isAnyDeffectsFound = true;
      break;
    }
  }

  int width = DDNV2Variables::inputWidth;
  int height = DDNV2Variables::inputHeight;
  cv::Mat answer = cv::Mat::zeros(cv::Size{ width, height }, CV_8U);

  if (isAnyDeffectsFound) {
    int layerSize = width * height;
    auto dataLayer0 = outputDecoder.data_ptr<float_t>();
    auto dataLayer1 = dataLayer0 + layerSize;
    auto dataLayer2 = dataLayer1 + layerSize;
    auto dataLayer3 = dataLayer2 + layerSize;

    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width; ++j) {
        if (dataLayer0[j] > DDNV2Variables::thresholds[0] ||
            dataLayer1[j] > DDNV2Variables::thresholds[1] ||
            dataLayer2[j] > DDNV2Variables::thresholds[2] ||
            dataLayer3[j] > DDNV2Variables::thresholds[3]) {
          answer.at<uchar>(i, j) = 255;
        }
      }

      dataLayer0 += width;
      dataLayer1 += width;
      dataLayer2 += width;
      dataLayer3 += width;
    }
  }
  return answer;
}

cv::Mat DefectDetectionNetworkV2::prepareForInfer(const cv::Mat &image,
                                                  double &scaleToAnswer)
{
  cv::Mat result;
  std::vector<cv::Mat> channels(3);

  if (image.type() == CV_8UC1) {
    channels = {image, image, image};
  } else if (image.type() == CV_8UC3) {
    cv::Mat imageGray;
    cv::cvtColor(image, imageGray, cv::COLOR_BGR2GRAY); // Attention! It is assumed that image is BGR!
    channels = {imageGray, imageGray, imageGray};
  } else if (image.type() == CV_16UC1) {
    cv::Mat imageMono8;
    constexpr double alpha = 1.0/256;
    image.convertTo(imageMono8, CV_8U, alpha);
    channels = {imageMono8, imageMono8, imageMono8};
  } else {
    fprintf(stderr, "DefectDetectionNetworkV2: doesn't "
                    "know this image format!\n");
    cv::Mat zeros = cv::Mat::zeros(image.size(), CV_8UC1);
    channels = {zeros, zeros, zeros};
  }
  // FIXME: there will be more types
  cv::merge(channels, result);
  //
  int tempWidth = image.cols;
  int tempHeight = image.cols * DDNV2Variables::inputRatio;
  if (tempHeight > image.rows) {
    tempHeight = image.rows;
    tempWidth = image.rows / DDNV2Variables::inputRatio;
  }
  scaleToAnswer = double(tempWidth) / DDNV2Variables::inputWidth;

  cv::Rect roi((image.cols - tempWidth) >> 1,
               (image.rows - tempHeight) >> 1, tempWidth, tempHeight);
  result = result(roi);
  cv::resize(result, result, cv::Size(DDNV2Variables::inputWidth,
          DDNV2Variables::inputHeight), 0, 0, cv::INTER_LINEAR);
  return result;
}