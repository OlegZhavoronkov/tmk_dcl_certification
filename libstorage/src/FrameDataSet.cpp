#include <storage/FrameDataSet.h>

namespace tmk::storage {

FrameDataSet::FrameDataSet(
                             const ipc::void_allocator &shmemAlloc
                           , const std::string &name_
                           , StorageType storageType_
                          )
                           : IObject(name_, storageType_)
                           , framesData(std::less<ObjectID>(), shmemAlloc)
                            {}

/*
json FrameDataSet::toJsonDB(const IDType &parentId) const {
  json ret = IObject::toJson();
  ret["object_id"] = parentId.id;
  return ret;
}
*/

std::vector<FrameDataID> FrameDataSet::framesDataIDs() {
  std::vector<FrameDataID> ret(framesData.size());
  std::transform(framesData.begin(), framesData.end(), ret.begin(),
                 [](const FramesDataValueType &v) { return v.first; });
  return ret;
}

size_t FrameDataSet::framesDataCount() const { return framesData.size(); }

FrameDataRef FrameDataSet::frameData(const FrameDataID &id) {
  if (framesData.contains(id)) {
    return {framesData.at(id)};
  }
  return {};
}

void FrameDataSet::removeFrameData(const FrameDataID &id) {
  if (framesData.contains(id)) {
    framesData.erase(id);
  }
}

/*
void FrameDataSet::fromJsonDB(const json &jFs) { IObject::fromJson(jFs); }

json FrameDataSet::toJson() const {
  json ret = IObject::toJson();
  ret["frames"] = json::array();

  for (auto &[frameDataId, frameData] : framesData) {
    if (frameData.storageType != storage::StorageType::NO_SAVE) {
      ret["frames"].push_back(frameData.toJson());
    }
  }
  return ret;
}

void FrameDataSet::fromJson(const json &jFs, const ipc::void_allocator &alloc) {
  IObject::fromJson(jFs);
  if (!jFs.contains("frames")) {
    LOG(ERROR) << "JSON object does not contain ObjectDescriptor structure";
    return;
  }
  for (auto &jFrame : jFs["frames"]) {
    auto name = jFrame["name"].get<std::string>();
    auto frame = addFrameData(alloc, name, storage::StorageType{});
    frame().fromJson(jFrame);
  }
}
*/

bool FrameDataSet::isValidFrameData(const FrameDataID &id) {
    return framesData.contains(id);
}

void FrameDataSet::show()
{
   LOG(INFO) << "\nFrameDataSet size: " << framesData.size();

   for(auto& elem : framesData)
   {
     elem.second.show();
   }
}

} // namespace tmk::storage
