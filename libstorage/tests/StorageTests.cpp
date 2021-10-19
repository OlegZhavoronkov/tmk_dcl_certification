#include <storage/Ipc.h>
#include <storage/ObjectDescriptor.h>

#include <gtest/gtest.h>

#include <sys/wait.h>

using namespace tmk;
using namespace tmk::storage;

bool imageFilled(const cv::Mat &image, const cv::Scalar &s) {
  for (cv::Vec3f &p : cv::Mat_<cv::Vec3f>(image)) {
    if (cv::Scalar(p) != s) {
      return false;
    }
  }
  return true;
}

bool checkSharedFrame(const std::string &name, const cv::Scalar &s) {
  auto pid = fork();
  if (pid == 0) {
    auto [segment, alloc] = ipc::openSharedMemory("MySharedFrames");

    auto myFrame1 = ipc::findShared<Frame>(segment, name);
    auto mat = myFrame1().image();
    exit(imageFilled(mat, s) ? 0 : 1);
  }
  int status;
  auto ret = waitpid(pid, &status, 0);
  if (ret < 0 || WEXITSTATUS(status) != 0) {
    return false;
  }
  return true;
}

TEST(SharedMemory, Frame) {
  // Remove shared memory on construction and destruction
  struct shm_remove {
    shm_remove() { ipc::destroySharedMemory("MySharedFrames"); }
    ~shm_remove() { ipc::destroySharedMemory("MySharedFrames"); }
  } remover;

  // Create and map shared memory, An allocator convertible to any allocator<T,
  // segment_manager_t> type
  auto [segment, shmemAlloc] =
      ipc::createSharedMemory("MySharedFrames",
                              1000000); // enough to 2 images

  TimeT tstamp = system_clock::now();
  int sequenceNum = 100;
  int source = 5;
  std::string name = "frame_with_some_data";
  StorageType st = StorageType::NO_SAVE;
  cv::Mat image(100, 301, CV_32FC3);

  image.setTo(cv::Scalar(5, 3, 1));
  auto myFrame1 = ipc::createShared<Frame>(segment, "test_img1", st, image,
                                           tstamp, sequenceNum, source);

  image.setTo(cv::Scalar(0, 2, 4));
  auto myFrame2 = ipc::createShared<Frame>(segment, "test_img2", st, image,
                                           tstamp, sequenceNum, source);

  auto myFrame3 = ipc::createShared<Frame>(segment, name, st, image, tstamp,
                                           sequenceNum, source);

  EXPECT_TRUE(myFrame1.valid());
  EXPECT_TRUE(myFrame2.valid());
  EXPECT_FALSE(myFrame3.valid());
  EXPECT_TRUE(checkSharedFrame("test_img1", cv::Scalar(5, 3, 1)));
  EXPECT_TRUE(checkSharedFrame("test_img2", cv::Scalar(0, 2, 4)));

  if (myFrame1)
    ipc::destroyShared(segment, myFrame1);
  if (myFrame2)
    ipc::destroyShared(segment, myFrame2);
}

TEST(SharedMemory, ObjectDescriptorClientFill) {
  // Remove shared memory on construction and destruction
  struct shm_remove {
    shm_remove() { ipc::destroySharedMemory("MySharedObjects"); }
    ~shm_remove() { ipc::destroySharedMemory("MySharedObjects"); }
  } remover;

  // Create shared memory
  auto [segment, shmemAlloc] =
      ipc::createSharedMemory("MySharedObjects", 800000); // enough to 2 images

  std::string name = "Pipe_123456";
  auto myObj = ipc::createShared<ObjectDescriptor>(
      segment, name, StorageType::NO_SAVE, ObservableType::PIPE);

  // fill the object in different process
  auto pid = fork();
  if (pid == 0) {
    auto [segment, clientShmemAlloc] = ipc::openSharedMemory("MySharedObjects");

    auto clientObj = ipc::findShared<ObjectDescriptor>(segment, name);

    std::cout << "Constructing Mat" << std::endl;

    TimeT tstamp = system_clock::now();
    int sequenceNum = 100;
    int source = 5;
    //StorageType st = StorageType::NO_SAVE;
    cv::Mat image(100, 301, CV_32FC3);
    // image.setTo(cv::Scalar(1, 2, 3));

    std::cout << "Constructing Frame" << std::endl;
    int ret = 0;

    try {
      // frameset inside object
      auto frameset = clientObj().addFrameset(clientShmemAlloc, "Frameset1",
                                              StorageType::NO_SAVE);
      image = cv::Scalar(4, 5, 6);
      frameset().addFrame(clientShmemAlloc, "1", StorageType::NO_SAVE, image,
                          tstamp, sequenceNum,
                          source); // 1 image in shared memory
      std::cout << "1 " << frameset().framesCount() << std::endl;
      // construct frameset outside of object
      image = cv::Scalar(1, 2, 3);
      auto fs = ipc::createShared<Frameset>(segment, "Frameset2",
                                            StorageType::NO_SAVE);
      std::cout << "shared frameset created" << std::endl;
      fs().addFrame(clientShmemAlloc, "2", StorageType::NO_SAVE, image, tstamp,
                    sequenceNum,
                    source); // 2 images in shared memory
      std::cout << "frame added" << std::endl;

      clientObj().addFrameset(std::move(fs())); // move (still 2 imgs)
      std::cout << "2 " << clientObj().framesetsCount() << " "
                << fs().framesCount() << std::endl;

      auto fs_check = ipc::findShared<Frameset>(segment, "Frameset2");
      if (!fs_check.valid() || fs_check().framesCount() != 0) {
        ret = 1;
      }
      // Named object should be destroyed after use
      ipc::destroyShared<Frameset>(segment, fs);
    } catch (const std::exception &e) {
      std::cout << e.what() << std::endl;
    }
    exit(ret);
  }

  int status;
  auto ret = waitpid(pid, &status, 0);
  ASSERT_TRUE(ret > 0 && WEXITSTATUS(status) == 0);
  ASSERT_EQ(myObj().framesetsCount(), 2);
  ASSERT_NE(myObj().frameset({"Frameset1"})().framesCount(), 0);
  ASSERT_TRUE(
      imageFilled(myObj().frameset({"Frameset1"})().frame({"1"})().image(),
                  cv::Scalar(4, 5, 6)));
  ASSERT_TRUE(
      imageFilled(myObj().frameset({"Frameset2"})().frame({"2"})().image(),
                  cv::Scalar(1, 2, 3)));

  // test deletion of frames with framesets
  // check object refill

  TimeT tstamp = system_clock::now();
  int sequenceNum = 100;
  int source = 5;
  //StorageType st = StorageType::NO_SAVE;
  cv::Mat image(100, 301, CV_32FC3);
  image.setTo(cv::Scalar(7, 8, 9));

  for (auto &id : myObj().framesetsIDs()) {
    myObj().removeFrameset(id);
  }
  auto fs = myObj().addFrameset(shmemAlloc, "Frameset1", StorageType::NO_SAVE);

  auto frame1 =
      fs().addFrame(shmemAlloc, "some_image_name1", StorageType::NO_SAVE, image,
                    tstamp, sequenceNum, source);
  ASSERT_TRUE(frame1.valid());
  auto frame2 =
      fs().addFrame(shmemAlloc, "some_image_name2", StorageType::NO_SAVE, image,
                    tstamp, sequenceNum, source);
  ASSERT_TRUE(frame2.valid());

  // there is no room for image 3
  LOG(INFO) << "Generating error:";
  auto frame3 =
      fs().addFrame(shmemAlloc, "some_image_name3", StorageType::NO_SAVE, image,
                    tstamp, sequenceNum, source);
  ASSERT_FALSE(frame3.valid());
}