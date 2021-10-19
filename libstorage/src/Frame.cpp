#include <storage/Frame.h>
#include <storage/Frameset.h>

#include <glog/logging.h>

#include <chrono>
#include <iomanip>
#include <iostream>

namespace tmk::storage {

Frame::Frame(
               const ipc::void_allocator &shmemAlloc
             , const std::string &name_
             , StorageType storageType_
             , const cv::Mat &image
             , TimeT timestamp_
             , int sequenceNum_
             , int source_
             , bool isSaveData_toDB_
             )
             : IObject(name_, storageType_)
             , timeStamp(timestamp_)
             , sequenceNum(sequenceNum_)
             , source(source_)
             , imageData(shmemAlloc)
             , m_isSaveData_toDB(isSaveData_toDB_) {
                setImage(image);
             }

Frame::Frame( 
               const ipc::void_allocator &shmemAlloc
             , const std::string &name_
             , StorageType storageType_
             )            : IObject(name_, storageType_),
              imageData(shmemAlloc) {}

void Frame::setImage(const cv::Mat &image) {
  cv::Mat img;
  if (!image.isContinuous()) {
    img = image.clone();
  } else {
    img = image;
  }
  width = image.cols;
  height = image.rows;
  matType = image.type();
  size_t dataSize = image.step[0] * image.rows;
  imageData.resize(dataSize);
  memcpy(imageData.data(), image.data, dataSize);
}

cv::Mat Frame::image() {
  if (imageData.empty()) {
    LOG(ERROR) << "Frame is not correctly initialized";
    return {};
  }
  cv::Mat ret{cv::Size{width, height}, matType, imageData.data()};
  return ret;
}

json Frame::toJsonDB(const FramesetID &parentId) const {
  json ret = IObject::toJson();

  auto time = std::chrono::system_clock::to_time_t(timeStamp);
  std::stringstream str;
  str << std::put_time(std::localtime(&time), "%F_%H:%M:%S");

  ret["datetime"] = str.str();
  ret["sequence"] = sequenceNum;
  ret["source"] = source;
  if (parentId.id != IMPOSSIBLE_ID) {
    ret["frameset_id"] = parentId.id;
  }
  return ret;
}

void Frame::fromJsonDB(const json &frame) {
  IObject::fromJson(frame);
  if (!frame.contains("datetime") || !frame.contains("sequence") ||
      !frame.contains("source")) {
    LOG(ERROR) << "JSON object does not contain Frame structure";
    return;
  }
  auto datetime = frame["datetime"].get<std::string>();
  std::tm time;
  std::get_time(&time, "%F_%H:%M:%S");
  timeStamp = system_clock::from_time_t(mktime(&time));
  sequenceNum = frame["sequence"].get<int>();
  source = frame["source"].get<int>();
}

json Frame::toJson() const { return toJsonDB(FramesetID{""}); }
void Frame::fromJson(const json &j) { fromJsonDB(j); }


void Frame::show() const
{
      //  std::cout << "\nTime        :" << matType;
      LOG(INFO) << "\nsequenceNum :" << sequenceNum;
      LOG(INFO) << "\nsource      :" << source;
      LOG(INFO) << "\nwidth       :" << width;
      LOG(INFO) << "\nheight      :" << height;
      LOG(INFO) << "\nmatType     :" << matType;
      LOG(INFO) << "\nsave_toDB   :" << m_isSaveData_toDB;
      LOG(INFO) << std::endl;

}


} // namespace tmk::storage
