#include <storage/ObjectDescriptor.h>

namespace tmk::storage {

std::ostream &operator<<(std::ostream &out, const ObjectID &id) {
  out << id.name;
  return out;
}

ObjectDescriptor::ObjectDescriptor(const ipc::void_allocator &shmemAlloc,
                                   const std::string &name_,
                                   StorageType storageType_,
                                   ObservableType type_)
    :  IObject(name_, storageType_), type(type_)
     , framesets(std::less<ObjectID>(), shmemAlloc)
     , framedatasets(std::less<ObjectID>(), shmemAlloc)                              // add by me for 
     , m_object_data(shmemAlloc, name_, storageType_)
      {}

std::vector<FramesetID> ObjectDescriptor::framesetsIDs() {
  std::vector<FramesetID> ret(framesets.size());
  std::transform(framesets.begin(), framesets.end(), ret.begin(),
                 [](const FramesetsValueType &v) { return v.first; });
  return ret;
}


std::vector<FrameDataSetID> ObjectDescriptor::frameDataSetsIDs() {
  std::vector<FrameDataSetID> ret(framedatasets.size());
  std::transform(framedatasets.begin(), framedatasets.end(), ret.begin(),
                 [](const FrameDataSetsValueType &v) { return v.first; });
  return ret;
}

FramesetRef ObjectDescriptor::addFrameset(Frameset &&frameset) {
  FramesetRef ref;
  auto id = frameset.id; // make copy to prevent case if move() happens first
  try {
    auto [iter, success] = framesets.emplace(id, std::move(frameset));
    ref = iter->second;
    if (!success) {
      LOG(WARNING) << "Frameset: " << ref().id << " already belongs to object "
                   << this->id;
    }
  } catch (const std::exception &e) {
    LOG(ERROR) << e.what();
  }
  return ref;
}

FrameDataSetRef ObjectDescriptor::addFrameDataSet(FrameDataSet &&framedataset){
  FrameDataSetRef ref;

  auto id = framedataset.id; // make copy to prevent case if move() happens first

  try {
    auto [iter, success] = framedatasets.emplace(id, std::move(framedataset));

    ref = iter->second;
    if (!success) {
      LOG(WARNING) << "FrameDataSet: " << ref().id << " already belongs to object "
                   << this->id;
    }

  } catch (const std::exception &e) {
    LOG(ERROR) << e.what();
  }

  return ref;
}

size_t ObjectDescriptor::framedatasetsCount() {return framedatasets.size(); }

void ObjectDescriptor::show()
{
   // show framesets
   LOG(INFO) << "\nObjectDescriptor have ";
   LOG(INFO) << "\nframeset: "     << framesets.size();
   LOG(INFO) << "\nframedataset: " << framedatasets.size();

   LOG(INFO) << "\nshow frameset: ";

   for (auto & elem: framesets)
   {
     elem.second.show();
   }

   LOG(INFO) << "\nshow framedataset: ";
   for (auto & elem: framedatasets)
   {
     elem.second.show();
   }

}

size_t ObjectDescriptor::framesetsCount() { return framesets.size(); }

FramesetRef ObjectDescriptor::frameset(const FramesetID &id) {
  if (framesets.contains(id)) {
    return framesets.at(id);
  }
  return {};
}

FrameDataSetRef ObjectDescriptor::framedataset(const FrameDataSetID &id) {
 if (framedatasets.contains(id)) {
    return framedatasets.at(id);
  }
  return {};
}

void ObjectDescriptor::removeFrameset(const FramesetID &id) {

    if (framesets.contains(id)) {
        framesets.erase(id);
       }

}

void ObjectDescriptor::removeFrameDataSet(const FrameDataSetID &id) {
    if (framedatasets.contains(id)) {
        framedatasets.erase(id);
       }
}

json ObjectDescriptor::toJson() const {
  json ret = IObject::toJson();

  ret["framesets"] = json::array();
  json &jsonFramesets = ret["framesets"];
  for (auto &[fsId, fs] : framesets) {
    if (fs.storageType != storage::StorageType::NO_SAVE) {
      jsonFramesets.push_back(fs.toJson());
    }
  }
  // TODO: Save all other stuff
  return ret;
}

void ObjectDescriptor::fromJson(const json &jObj,
                                const ipc::void_allocator &alloc) {
  IObject::fromJson(jObj);
  if (!jObj.contains(
          "framesets")) { // TODO: check other key values to prevent exceptions
    LOG(ERROR) << "JSON object does not contain ObjectDescriptor structure";
    return;
  }

  for (auto &jFs : jObj["framesets"]) {
    auto name = jFs["name"].get<std::string>();
    auto fs = addFrameset(alloc, name, storage::StorageType{});
    fs().fromJson(jFs, alloc);
  }
  // TODO: Load all other stuff
}

bool ObjectDescriptor::isValidFrameset(const FramesetID &id)
{
   return framesets.contains(id);
}

} // namespace tmk::storage

