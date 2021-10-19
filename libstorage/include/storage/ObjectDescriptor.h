#pragma once

#ifndef STORAGE_OBJECTDESCRIPTOR_H
#define STORAGE_OBJECTDESCRIPTOR_H

#include <storage/StorageTypes.h>
#include <storage/Frameset.h>
#include <storage/FrameDataSet.h>
#include <storage/ObjectData.h>

#include <optional>

namespace  tmk::storage{
    class ObjectData;
}

namespace tmk::storage {

struct ObjectDescriptorID : public ObjectID {
  using ObjectID::ObjectID;
};

using ObjectDescriptorRef = ObjectRef<class ObjectDescriptor>;

class ObjectDescriptor : public IObject<ObjectDescriptorID> {
public:
  // Do NOT use this constructor standalone!
  // use createShared() instead
  ObjectDescriptor(const ipc::void_allocator &shmemAlloc,
                   const std::string &name_, StorageType storageType_,
                   ObservableType type_);

  /**
   * @brief create new Frameset into ObjectDescriptor internal storage
   * @param shmemAlloc allocator for shared memory
   * typically
   * ipc::void_allocator{ipc::managed_shared_memory{...}.get_segment_manager()}
   * @param name the name of frameset, should be unique inside ObjectDescriptor
   * @param storageType of type StorageType
   */
  template <typename... Args>
  FramesetRef addFrameset(const ipc::void_allocator &shmemAlloc,
                          const std::string &name, Args &&...args) {
    FramesetRef ref;
    try {
      auto [iter, success] = framesets.emplace(
          name, Frameset{shmemAlloc, name, std::forward<Args>(args)...});
      ref = iter->second;
      if (!success) {
        LOG(WARNING) << "Frameset: " << ref().id
                     << " already belongs to object " << this->id;
      }
    } catch (const std::exception &e) {
      LOG(ERROR) << e.what();
    }
    return ref;
  }

  // emplaces existing frameset into ObjectDescriptor internal storage
  // no Frame data copy should occur
  FramesetRef addFrameset(Frameset &&frameset);

  size_t framesetsCount();
  std::vector<FramesetID> framesetsIDs();
  FramesetRef frameset(const FramesetID &id);
  void removeFrameset(const FramesetID &id);

  json toJson() const;
  void fromJson(const json &, const ipc::void_allocator &shmemAlloc);

  //============== add func 21.04.2021 ============
  bool isValidFrameset(const FramesetID &id);

  template <typename... Args>
  FrameDataSetRef addFrameDataSet(const ipc::void_allocator &shmemAlloc,
                          const std::string &name, Args &&...args) {
  FrameDataSetRef ref;
    try {
      auto [iter, success] = framedatasets.emplace(
          name, FrameDataSet{shmemAlloc, name, std::forward<Args>(args)...});
      ref = iter->second;
      if (!success) {
        LOG(WARNING) << "FrameDataSet: " << ref().id
                     << " already belongs to object " << this->id;
      }
    } catch (const std::exception &e) {
      LOG(ERROR) << e.what();
    }
    return ref;
  }

  // emplaces existing framedataset into ObjectDescriptor internal storage
  FrameDataSetRef addFrameDataSet(FrameDataSet &&framedataset);

  // get size of container framedatasets
  size_t framedatasetsCount();

  // get container with collection FrameDataSetID
  std::vector<FrameDataSetID> frameDataSetsIDs();

  // get a reference on FrameDataSet by name of FrameDataSet id
  FrameDataSetRef framedataset(const FrameDataSetID &id);

  void removeFrameDataSet(const FrameDataSetID &id);

  // show debug info how many frameset in objectdescriptor
  void show();

 // get type of the ObjectDescription:  BILLET, MANDREL, PIPE, PIPE_COOLED
  ObservableType get_ObservableType() const {return type;}

  //============== add func 21.04.2021 ============
  protected:
  using FramesetsValueType = std::pair<const FramesetID, Frameset>;
  using FramesetsAllocator = boost::interprocess::allocator<FramesetsValueType, ipc::segment_manager>;
  using Framesets =
      boost::interprocess::map<FramesetID, Frameset, std::less<ObjectID>, FramesetsAllocator>;

  ObjectData m_object_data;
  ObservableType type;
  Framesets framesets;
//  Framesets framesets2;


//--------------------- add FrameDataSet ---------------------------
 using FrameDataSetsValueType = std::pair<const FrameDataSetID, FrameDataSet>;       
 using FrameDataSetsAllocator =
       boost::interprocess::allocator<FrameDataSetsValueType, ipc::segment_manager>;
 using FrameDataSets   = boost::interprocess::map<FrameDataSetID, FrameDataSet, std::less<ObjectID>, FrameDataSetsAllocator>;

 FrameDataSets framedatasets;

//--------------------- add FrameDataSet --------------------------- 


//--------------------- add FrameDataSet ---------------------------
  // TODO: add other data describing the purpose of frames in framesets and
  // calculations results
};

} // namespace tmk::storage
#endif // STORAGE_OBJECTDESCRIPTOR_H
