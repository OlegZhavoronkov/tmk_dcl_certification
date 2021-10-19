#include <dbpg/DescriptorsDB.h>
#include <dbpg/Filesystem.h>

#include <gtest/gtest.h>
#include <pqxx/pqxx>

using namespace tmk;
using namespace tmk::dbpg;
using namespace tmk::storage;

fs::path getTestLocalDataPath(const fs::path &relative_path, bool cleanup) {
  const auto desired_path = fs::path(TEST_LOCAL_DATA_DIR) / relative_path;

  if (fs::exists(desired_path)) {
    if (cleanup) {
      try {
        LOG(INFO) << "Removing " << desired_path;
        fs::remove_all(desired_path);
      } catch (const std::exception &ex) {
        if (!fs::is_empty(desired_path))
          throw ex;
      }
    }
  }

  if (!fs::exists(desired_path)) {
    if (!fs::create_directories(desired_path)) {
      LOG(ERROR) << "Couldn't create '" << desired_path << "' directory";
      return "";
    }
  }

  return desired_path;
}

static const std::string shmemName{"test_mem"};

TEST(FsStorageTests, saveAndLoad) {
  struct ShmemRemover {
    ShmemRemover() { ipc::destroySharedMemory(shmemName); }
    ~ShmemRemover() { ipc::destroySharedMemory(shmemName); }
  } remover;
  auto [segment, alloc] = ipc::createSharedMemory(shmemName, 1000000);

  auto obj = ipc::createShared<ObjectDescriptor>(
      segment, "test_obj", StorageType::SHORT_TERM, ObservableType::PIPE);

  auto fs = obj().addFrameset(alloc, "test_fs", StorageType::SHORT_TERM);

  TimeT tstamp = system_clock::now();
  int sequenceNum = 100;
  int source = 5;
  std::string name = "frame_with_some_data";
  StorageType st = StorageType::LONG_TERM;
  cv::Mat image(100, 301, CV_32FC3);
  image = cv::Scalar(1, 2, 3);
  fs().addFrame(alloc, name, st, image, tstamp, sequenceNum, source);
  fs().addFrame(alloc, name + "_no_save", StorageType::NO_SAVE, image, tstamp,
                sequenceNum, source);

  auto fs_no_save = obj().addFrameset(alloc, "test_fs2", StorageType::NO_SAVE);
  image = cv::Scalar(3, 2, 1);
  fs().addFrame(alloc, name, st, image, tstamp, sequenceNum, source);

  DescriptorsDB db{shmemName, getTestLocalDataPath("dbfs", true)};
  db.save({"test_obj"});
  db.removeFromSharedMemory({"test_obj"});

  DescriptorsDB db2{shmemName, getTestLocalDataPath("dbfs", false)};
  auto loaded = db2.load({"test_obj"});
  ASSERT_TRUE(std::string(loaded().id.name) == "test_obj");
  ASSERT_TRUE(loaded().framesetsCount() == 1);
  ASSERT_TRUE(loaded().storageType == StorageType::SHORT_TERM);
  auto fs_test = loaded().frameset({"test_fs"});
  ASSERT_TRUE(fs_test().storageType == StorageType::SHORT_TERM);

  auto frame = fs_test().frame({"frame_with_some_data"});
  ASSERT_TRUE(frame().storageType == StorageType::LONG_TERM);
  auto image_test = frame().image();
  ASSERT_EQ(image_test.size(), cv::Size2i(301, 100));
  ASSERT_TRUE(image_test.at<cv::Vec3f>(10, 50) == cv::Vec3f(1.f, 2.f, 3.f));
}