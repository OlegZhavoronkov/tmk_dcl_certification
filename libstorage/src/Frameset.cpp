#include <storage/Frameset.h>

namespace tmk::storage {

Frameset::Frameset(const ipc::void_allocator &shmemAlloc,
                   const std::string &name_, StorageType storageType_)
    : IObject(name_, storageType), frames(std::less<ObjectID>(), shmemAlloc) {}

json Frameset::toJsonDB(const IDType &parentId) const {
  json ret = IObject::toJson();
  ret["object_id"] = parentId.id;
  return ret;
}

std::vector<FrameID> Frameset::framesIDs() {
  std::vector<FrameID> ret(frames.size());
  std::transform(frames.begin(), frames.end(), ret.begin(),
                 [](const FramesValueType &v) { return v.first; });
  return ret;
}

size_t Frameset::framesCount() const { return frames.size(); }

FrameRef Frameset::frame(const FrameID &id) {
  if (frames.contains(id)) {
    return {frames.at(id)};
  }
  return {};
}

void Frameset::removeFrame(const FrameID &id) {
  if (frames.contains(id)) {
    frames.erase(id);
  }
}

void Frameset::fromJsonDB(const json &jFs) { IObject::fromJson(jFs); }

json Frameset::toJson() const {
  json ret = IObject::toJson();
  ret["frames"] = json::array();

  for (auto &[frameId, frame] : frames) {
    if (frame.storageType != storage::StorageType::NO_SAVE) {
      ret["frames"].push_back(frame.toJson());
    }
  }
  return ret;
}

void Frameset::fromJson(const json &jFs, const ipc::void_allocator &alloc) {
  IObject::fromJson(jFs);
  if (!jFs.contains("frames")) {
    LOG(ERROR) << "JSON object does not contain ObjectDescriptor structure";
    return;
  }
  for (auto &jFrame : jFs["frames"]) {
    auto name = jFrame["name"].get<std::string>();
    auto frame = addFrame(alloc, name, storage::StorageType{});
    frame().fromJson(jFrame);
  }
}

bool Frameset::isValidFrame(const FrameID &id) {
    return frames.contains(id);
}

// ================ !!! my add  for Debug =================================
// show property from Frame
void Frameset::show(const FrameID &id) const {

  if (frames.contains(id))
  {

    auto  it = frames.find(id);
    // get Frame.show();
   (it->second).show();

  }

}

 void Frameset::show() const {

  LOG(INFO) << "\nFrameset size: " << frames.size();
  for (auto& elem : frames)
  {
    elem.second.show();
  }

 }

// ================ !!! my add  for Debug =================================

} // namespace tmk::storage
