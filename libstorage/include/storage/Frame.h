//------------------------------------------------------------------------------
//  Created     : 01.01.2021
//  Author      : Ekaterina Mosseyko, Victor Kats ...
//  ver         : 0.0.1
//  Description : Frame - describ
//
//------------------------------------------------------------------------------
#pragma once

#ifndef STORAGE_FRAME_H
#define STORAGE_FRAME_H

#include <storage/IObject.h>
#include <storage/Time.h>

#include <opencv2/core.hpp>

#include <vector>

namespace tmk::storage {

/**
 * @struct FrameID - struct for storing data: name and id
*/
struct FrameID : public ObjectID {
  using ObjectID::ObjectID;
};

struct FramesetID;

/**
 * @class Frame - structure describe polarizing image
 * polarizing image - consist from 4 images
*/
class Frame : public IObject<FrameID> {
public:
  // Do NOT use this constructor standalone!
  // use ipc::createShared() or use Frameset::addFrame() instead
  //

  //Kat's comment: for C-interface w/o opencv necessary something as this for Frameset::addFrame()
  /* Think about...
  Frame(
          const ipc::void_allocator &shmemAlloc
        , const std::string &name_
        , StorageType storageType_
        , int width
        , int height
        , int matType
        , const char *frame_data // or any ptr to pure source frame data -- array of bytes
                                 // --> copy to ipc::buffer imageData; 
                                 // this is unique location for copy from socket to boost shmem
        , TimeT timestamp_
        , int sequenceNum_
        , int source_
        , bool isSaveData_toDB_ = false      // by default, we do not save Frame in the database
        );
  */

  Frame(
          const ipc::void_allocator &shmemAlloc
        , const std::string &name_
        , StorageType storageType_
        , const cv::Mat &image_
        , TimeT timestamp_
        , int sequenceNum_
        , int source_
        , bool isSaveData_toDB_ = false      // by default, we do not save Frame in the database
        );

  Frame(
          const ipc::void_allocator &shmemAlloc
        , const std::string &name
        , StorageType storageType_
        );

  json toJsonDB(const FramesetID &parentId) const;
  void fromJsonDB(const json &);

  json toJson() const;
  void fromJson(const json &);

  cv::Mat image();
  void setImage(const cv::Mat &);

 // ================= New funcs [1]=============================
  bool isSaveData_toDB() const {return m_isSaveData_toDB;}                                        // getter for m_isSaveData_toDB
  void set_isSaveData_toDB(const bool& saveData_toDB) {m_isSaveData_toDB = saveData_toDB;}        // setter for m_isSaveData_toDB
  void show() const;                                                                              // for Debug to see data in Frame
 // ================= New funcs [2]=============================

  // ipc::string filePath;
  TimeT timeStamp;
  int sequenceNum;
  int source;

protected:
  ipc::buffer imageData;
  int width;
  int height;
  int matType;
  bool m_isSaveData_toDB;            // the mark that you need to save  data of the Frame and FrameData to the DB
  // FrameData& frameData;           // addition data of picture
  // CameraSettings& cameraSettings; // addition data of setting camera 
                                     // at which the frame was shooted
};
} // namespace tmk::storage

#endif // STORAGE_FRAME_H
