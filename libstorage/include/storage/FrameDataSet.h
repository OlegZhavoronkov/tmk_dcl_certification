//------------------------------------------------------------------------------
//  Created     : 22.04.2021
//  Author      : Vadim Yarmushov ...
//  ver         : 0.0.1
//  Description : FrameDataSet.h - define struct  to collect all Frames
//
//------------------------------------------------------------------------------
#pragma once

#ifndef STORAGE_FRAMEDATASET_H
#define STORAGE_FRAMEDATASET_H

#include <storage/FrameData.h>

namespace tmk::storage 
{

  struct FrameDataSetID : public ObjectID {
    using ObjectID::ObjectID;
  };

  using FrameDataRef    = ObjectRef<FrameData>;
  using FrameDataSetRef = ObjectRef<class FrameDataSet>;

  class FrameDataSet : public IObject<FrameDataSetID> {

    public:

      // Do NOT use this constructor standalone!
      // use ipc::createShared() or use ObjectDescriptor::addFrameDataSet() instead
      FrameDataSet(
                     const ipc::void_allocator &shmemAlloc
                   , const std::string &name_
                   , StorageType storageType_
                  );

      // TODO make func  toJsonDB(), toJson() to save some info in *.json
      /*
        json toJsonDB(const IDType &parentId) const;
        void fromJsonDB(const json &j);

        json toJson() const;
        void fromJson(const json &j, const ipc::void_allocator &shmemAlloc);
      */
        /**
        * @brief put image data into Frameset
        * @param shmemAlloc allocator for shared memory
        * typically
        * ipc::void_allocator{ipc::managed_shared_memory{}.get_segment_manager()}
        * @param name the name of image, should be unique inside Frameset
        * @param storageType of type StorageType
        * @param image of type cv::Mat - image to be copied into shared memory Frame
        * object
        * @param timestamp of type TimeT
        * @param sequenceNum of type int
        * @param source of type int
        */
      template <typename... Args>
      FrameDataRef addFrameData (
                                   const ipc::void_allocator &shmemAlloc
                                 , const std::string &name, Args &&...args) {
        FrameDataRef framedata;
        try {
          auto [iter, success] = framesData.emplace(
          FrameDataID{name},
          FrameData{shmemAlloc/*, t_vec_Defects*/, name, std::forward<Args>(args)...});
          framedata = iter->second;

          if (!success) {
            LOG(WARNING) << "FrameData: " << framedata().id
                         << " already in framedataset " << this->id;
          }
        }
        catch (const std::exception &e) {
          LOG(ERROR) << typeid(e).name()
                     << e.what();
        }

        if (!framedata.valid()) {
          LOG(ERROR) << "No shared memory available for Frame \""
                     << name << "\"";
        }

      return framedata;
      }

      size_t framesDataCount() const;

      FrameDataRef frameData(const FrameDataID &id);

      void removeFrameData(const FrameDataID &id);

      std::vector<FrameDataID> framesDataIDs();

      bool isValidFrameData(const FrameDataID &id);

      void show();

      protected:
        using FramesDataValueType = std::pair<const FrameDataID, FrameData>;
        using FramesDataAllocator = 
                            boost::interprocess::allocator<FramesDataValueType, ipc::segment_manager>;
        using Frames_data = boost::interprocess::map<FrameDataID, FrameData, std::less<ObjectID>,
                                                             FramesDataAllocator>;

        Frames_data framesData;
  };  // end class FrameDataSet


} //end  namespace tmk::storage


#endif // STORAGE_FRAMEDATASET_H