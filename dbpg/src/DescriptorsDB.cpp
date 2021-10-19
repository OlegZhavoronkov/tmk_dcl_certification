#include <dbpg/DescriptorsDB.h>

#include <nlohmann/json.hpp>

#include <fstream>

namespace tmk::dbpg {

void DescriptorsDB::removeFromSharedMemory(
    const storage::ObjectDescriptorID &id) {
  ipc::destroyShared<storage::ObjectDescriptor>(std::get<0>(sharedMemory), id.name);
}

bool DescriptorsDB::save(const storage::ObjectDescriptorID &id) {
  auto objRef =
      ipc::findShared<storage::ObjectDescriptor>(std::get<0>(sharedMemory), id.name);
  if (!objRef) {
    LOG(ERROR) << "ObjectDescriptor object " << id
               << " not found in shared memory";
    return false;
  }
  json objJson = objRef().toJson();

  std::string name{objRef().id.name};
  fs::path root = dbfsRootDir / name;
  fs::create_directories(root);

  fs::path jsonPath = root / desctiptorJsonFilename;
  std::ofstream out{jsonPath};
  out << objJson.dump(2);
  out.close();

  auto fsIds = objRef().framesetsIDs();
  for (auto &fsId : fsIds) {
    auto fs = objRef().frameset(fsId);
    if (fs().storageType != storage::StorageType::NO_SAVE) {
      auto fIds = fs().framesIDs();
      fs::path fsPath = root / fs().id.name;
      fs::create_directory(fsPath);
      for (auto &fId : fIds) {
        auto frame = fs().frame(fId);
        if (frame().storageType != storage::StorageType::NO_SAVE) {
          fs::path imgPath = fsPath / frame().id.name;
          cv::Mat image = frame().image();
          saveImage(image, imgPath);
        }
      }
    }
  }

  return true;
}

storage::ObjectDescriptorRef
DescriptorsDB::load(const storage::ObjectDescriptorID &id) {
  auto &[segment, alloc] = sharedMemory;
  auto objTest = ipc::findShared<storage::ObjectDescriptor>(segment, id.name);
  if (objTest) {
    LOG(WARNING) << "Object " << id << " already loaded";
    return objTest;
  }

  auto root = dbfsRootDir / id.name;
  fs::path jsonPath = root / desctiptorJsonFilename;
  if (!fs::exists(jsonPath)) {
    LOG(ERROR) << "Object " << id << " not found within " << dbfsRootDir;
    return {};
  }

  std::ifstream file{jsonPath};
  json objJson = json::parse(file);

  auto objRef = ipc::createShared<storage::ObjectDescriptor>(
      segment, id.name, storage::StorageType{}, storage::ObservableType{});
  objRef().fromJson(objJson, alloc);

  int framesCount = 0;
  for (auto &fsId : objRef().framesetsIDs()) {
    auto fsRef = objRef().frameset(fsId);
    fs::path fsPath = root / fsRef().id.name;
    for (auto &fId : fsRef().framesIDs()) {
      auto frameRef = fsRef().frame(fId);
      fs::path imgPath = fsPath / frameRef().id.name;
      cv::Mat image = readImage(imgPath);
      if (image.empty()) {
        LOG(ERROR) << "Image readind failed: " << imgPath;
        frameRef().setImage({}); // clear frame width height values
      } else {
        frameRef().setImage(image);
        framesCount++;
      }
    }
  }
  LOG(INFO) << "Object " << id << " loaded with " << objRef().framesetsCount()
            << " framesets, total " << framesCount << " frames";
  return objRef;
}

} // namespace tmk::dbpg
