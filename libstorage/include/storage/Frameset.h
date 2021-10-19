#pragma once

#ifndef STORAGE_FRAMESET_H
#define STORAGE_FRAMESET_H

#include <storage/Frame.h>

namespace tmk::storage {

  struct FramesetID : public ObjectID {
      using ObjectID::ObjectID;
  };

  using FrameRef = ObjectRef<Frame>;
  using FramesetRef = ObjectRef<class Frameset>;

  class Frameset : public IObject<FramesetID> {

    public:
    // Do NOT use this constructor standalone!
    // use ipc::createShared() or use ObjectDescriptor::addFrameset() instead
    Frameset(const ipc::void_allocator &shmemAlloc, const std::string &name_,
             StorageType storageType_);

    json toJsonDB(const IDType &parentId) const;
    void fromJsonDB(const json &j);

    json toJson() const;
    void fromJson(const json &j, const ipc::void_allocator &shmemAlloc);

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
    FrameRef addFrame(const ipc::void_allocator &shmemAlloc,
                    const std::string &name, Args &&...args) {
    FrameRef frame;
    try {
        auto [iter, success] = frames.emplace(
            FrameID{name},
            Frame{shmemAlloc, name, std::forward<Args>(args)...});
        frame = iter->second;
        if (!success) {
          LOG(WARNING) << "Frame: " << frame().id << " already in frameset "
                       << this->id;
        }
    } catch (const std::exception &e) {
      LOG(ERROR) << typeid(e).name() << e.what();
    }

    if (!frame.valid()) {
        LOG(ERROR) << "No shared memory available for Frame \"" << name << "\"";
    }

  return frame;
  } // end class Frameset

  size_t framesCount() const;

  FrameRef frame(const FrameID &id);

  void removeFrame(const FrameID &id);

  std::vector<FrameID> framesIDs();

  bool isValidFrame(const FrameID &id);

// ================ !!! my add  for Debug =================================
  // show property from Frame
  // FrameID &id -the name of the frame, the search in the frames
  // is performed by this key name
  void show(const FrameID &id) const;

// show all Frames in frames
  void show() const;
// ================ !!! my add  for Debug =================================



protected:

  using FramesValueType = std::pair<const FrameID, Frame>;
  using FramesAllocator = boost::interprocess::allocator<FramesValueType, ipc::segment_manager>;
  using Frames = boost::interprocess::map<FrameID, Frame, std::less<ObjectID>, FramesAllocator>;

  Frames frames;
};


} // namespace tmk::storage
#endif // !STORAGE_FRAMESET_H
