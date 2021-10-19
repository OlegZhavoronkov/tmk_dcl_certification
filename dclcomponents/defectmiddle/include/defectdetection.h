//-----------------------------------------------------------------------
//  Created     : 08.07.20
//  Author      : Ivan Kuznetsov, Yuliya Golovchanskaya
//  Description : Defect Detection Network
//-----------------------------------------------------------------------

#ifndef SFP_DEFECTDETECTION_H
#define SFP_DEFECTDETECTION_H

#include <string>
#include <opencv2/opencv.hpp> // cv::Mat

class IDefectDetectionNetwork
{
public:
  IDefectDetectionNetwork()=default;
  virtual ~IDefectDetectionNetwork()=default;
  virtual int load(const std::string &path)=0;
  virtual cv::Mat getDefectMask(const cv::Mat &image)=0;

private:
  virtual cv::Mat infer(const cv::Mat &image)=0;
  virtual cv::Mat prepareForInfer(const cv::Mat &image,
                                  double &scaleToAnswer)=0;
};

class DefectDetectionNetworkV1 : public IDefectDetectionNetwork
{
public:
  DefectDetectionNetworkV1()=default;
  ~DefectDetectionNetworkV1()=default;
  int load(const std::string &pathToFile) override;
  cv::Mat getDefectMask(const cv::Mat &image) override;

private:
  cv::Mat infer(const cv::Mat &image) override;
  cv::Mat prepareForInfer(const cv::Mat &image, double &scaleToAnswer) override;
};

class DefectDetectionNetworkV2 : public IDefectDetectionNetwork
{
  /*
   * In this version we use decoder and encoder  separately.
   * It should be much faster.
   */
public:
  DefectDetectionNetworkV2()=default;
  ~DefectDetectionNetworkV2()=default;
  int load(const std::string &pathToFolder) override;
  cv::Mat getDefectMask(const cv::Mat &image) override;

private:
  cv::Mat infer(const cv::Mat &image) override;
  cv::Mat prepareForInfer(const cv::Mat &image, double &scaleToAnswer) override;
};
#endif //SFP_DEFECTDETECTION_H
