#include "savemiddle.h"

#include "dclprocessor/dclprocessor.h"
#include "tcpcapture/cvcollector.h"
#include <storage/ObjectDescriptor.h>
#include <storage/ObjectRef.h>
#include <storage/Ipc.h>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <gtest/gtest.h>

#include <unistd.h> // access
#include <cstdio> // std::remove
#include <string>

using namespace tmk;
using namespace tmk::ipc;
using namespace tmk::storage;

namespace SaveConstants {
const std::string prefix = SAVEMIDDLE_TEST_DATA;
const std::string pathToSave = "../../../data/savemiddle/"; // from testmiddle.json
const std::string testConfigPath = pathToSave + "/testsave.json";
const std::string img = prefix + "/image.jpg";
const std::string img1 = prefix + "/defects/segmap_22_IMG_9964214_crop.jpg";
const std::string img2 = prefix + "/defects/segmap_34_IMG_9964217_crop.jpg";
const std::string img3 = prefix + "/defects/segmap_42_IMG_9964219_crop.jpg";
const std::string img4 = prefix + "/defects/segmap_58_IMG_996424_crop.jpg";
const std::string img5 = prefix + "/defects/segmap_62_IMG_996425_crop.jpg";
} // SaveConstants

TEST(MiddleBlockTest, ConfigTest) {
    saveMiddleConfig(SaveConstants::testConfigPath);
}

TEST(MiddleBlockTest, NoPictureTest) {
    cv::Mat image;
    image = cv::Mat::zeros(200, 200, CV_8UC3);
    struct shm_remove {
       shm_remove() {
           ipc::destroySharedMemory("TestSave");
       }
       ~shm_remove() {
           ipc::destroySharedMemory("TestSave");
       }
    } remover;
    auto [managed_shm, shmemAlloc] =
      tmk::ipc::createSharedMemory("TestSave", 200000);
    auto myObj = ipc::createShared<ObjectDescriptor>(managed_shm,
                                                     "aaa",
                                                     StorageType::NO_SAVE,
                                                     ObservableType::PIPE);
    auto frameset = myObj().addFrameset(shmemAlloc, "Not_image",
                                        StorageType::NO_SAVE);

    frameset().addFrame(shmemAlloc, "not_image", StorageType::NO_SAVE,
                        image, system_clock::now(), 100, 5);

    auto start = std::chrono::high_resolution_clock::now();
    SaveBlock(myObj, (char *)"boost_path", (char *)"aaa");
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        stop - start);
    std::cout << "Time taken saving one cv::Mat (SaveBlock): "
              << duration.count() << "ms." << std::endl;

    ipc::destroyShared<ObjectDescriptor>(managed_shm, myObj);
    ASSERT_FALSE(
      access((SaveConstants::pathToSave + "/Not_image/not_image.jpeg").c_str(),
             0));
    // clean up
    ASSERT_FALSE(std::remove((SaveConstants::pathToSave +
                             "/Not_image/not_image.jpeg").c_str()));
    ASSERT_FALSE(std::remove((SaveConstants::pathToSave +
                              "/Not_image").c_str()));
}

TEST(MiddleBlockTest, SmallSavingTest) {
    if (access(SaveConstants::img.c_str(), 0) != 0) {
        GTEST_SKIP() << "Didn't find picture, skipping test";
    }
    cv::Mat image = cv::imread(SaveConstants::img, 0);
    cv::imshow("an image to be saved", image);
    cv::waitKey(2000);

    struct shm_remove {
       shm_remove() {
           ipc::destroySharedMemory("TestSave");
       }
       ~shm_remove() {
           ipc::destroySharedMemory("TestSave");
       }
    } remover;
    auto [managed_shm, shmemAlloc] =
      tmk::ipc::createSharedMemory("TestSave", 2500000);
    auto myObj = ipc::createShared<ObjectDescriptor>(managed_shm,
                                                     "aaa",
                                                     StorageType::NO_SAVE,
                                                     ObservableType::PIPE);
    auto frameset = myObj().addFrameset(shmemAlloc, "Single_image",
                                        StorageType::NO_SAVE);

    frameset().addFrame(shmemAlloc, "single_image", StorageType::NO_SAVE,
                        image, system_clock::now(), 100, 5);

    auto start = std::chrono::high_resolution_clock::now();  
    SaveBlock(myObj, (char *)"boost_path", (char *)"aaa");
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        stop - start);
    std::cout << "Time taken saving one image (SaveBlock): "
              << duration.count() << "ms." << std::endl;         


    ipc::destroyShared<ObjectDescriptor>(managed_shm, myObj);
    ASSERT_FALSE(
      access((SaveConstants::pathToSave +
              "/Single_image/single_image.jpeg").c_str(),
             0));
    // clean up
    ASSERT_FALSE(std::remove((SaveConstants::pathToSave +
                             "/Single_image/single_image.jpeg").c_str()));
    ASSERT_FALSE(std::remove((SaveConstants::pathToSave +
                              "/Single_image").c_str()));
}

TEST(MiddleBlockTest, BigSavingTest) {
    if ((access(SaveConstants::img1.c_str(), 0) != 0) ||
        (access(SaveConstants::img2.c_str(), 0) != 0) ||
        (access(SaveConstants::img3.c_str(), 0) != 0) ||
        (access(SaveConstants::img4.c_str(), 0) != 0) ||
        (access(SaveConstants::img5.c_str(), 0) != 0)) {
        GTEST_SKIP() << "Didn't find one or more pictures, skipping test";
    }
    cv::Mat image1 = cv::imread(SaveConstants::img1, 0);
    cv::Mat image2 = cv::imread(SaveConstants::img2, 0);
    cv::Mat image3 = cv::imread(SaveConstants::img3, 0);
    cv::Mat image4 = cv::imread(SaveConstants::img4, 0);
    cv::Mat image5 = cv::imread(SaveConstants::img5, 0);

    struct shm_remove {
       shm_remove() {
           ipc::destroySharedMemory("TestSave");
       }
       ~shm_remove() {
           ipc::destroySharedMemory("TestSave");
       }
    } remover;

    auto [managed_shm, shmemAlloc] =
      tmk::ipc::createSharedMemory("TestSave", 6400000);
    auto myObj = ipc::createShared<ObjectDescriptor>(managed_shm,
                                                     "aaa",
                                                     StorageType::NO_SAVE,
                                                     ObservableType::PIPE);
    auto frameset = myObj().addFrameset(shmemAlloc, "FramesetTest",
                                        StorageType::NO_SAVE);
    frameset().addFrame(shmemAlloc, "1", StorageType::NO_SAVE,
                        image1, std::chrono::system_clock::now(), 100, 15);
    frameset().addFrame(shmemAlloc, "2", StorageType::NO_SAVE,
                        image2, std::chrono::system_clock::now(), 100, 5);
    frameset().addFrame(shmemAlloc, "3", StorageType::NO_SAVE,
                        image3, std::chrono::system_clock::now(), 100, 5);
    frameset().addFrame(shmemAlloc, "4", StorageType::NO_SAVE,
                        image4, std::chrono::system_clock::now(), 100, 15);
    frameset().addFrame(shmemAlloc, "5", StorageType::NO_SAVE,
                        image5, std::chrono::system_clock::now(), 100, 5);

    auto start = std::chrono::high_resolution_clock::now();  
    SaveBlock(myObj, (char *)"boost_path", (char *)"aaa");
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        stop - start);
    std::cout << "Time taken saving three images (SaveBlock): "
              << duration.count() << "ms." << std::endl;         

    ipc::destroyShared<ObjectDescriptor>(managed_shm, myObj);
    ASSERT_TRUE(
      access((SaveConstants::pathToSave + "/FramesetTest/1.jpeg").c_str(), 0));
    ASSERT_FALSE(
      access((SaveConstants::pathToSave + "/FramesetTest/2.jpeg").c_str(), 0));
    ASSERT_FALSE(
      access((SaveConstants::pathToSave + "/FramesetTest/3.jpeg").c_str(), 0));
    ASSERT_TRUE(
      access((SaveConstants::pathToSave + "/FramesetTest/4.jpeg").c_str(), 0));
    ASSERT_FALSE(
      access((SaveConstants::pathToSave + "/FramesetTest/5.jpeg").c_str(), 0));
    // clean up
    ASSERT_TRUE(std::remove((SaveConstants::pathToSave +
                             "/FramesetTest/1.jpeg").c_str()));
    ASSERT_FALSE(std::remove((SaveConstants::pathToSave +
                             "/FramesetTest/2.jpeg").c_str()));
    ASSERT_FALSE(std::remove((SaveConstants::pathToSave +
                             "/FramesetTest/3.jpeg").c_str()));
    ASSERT_TRUE(std::remove((SaveConstants::pathToSave +
                             "/FramesetTest/4.jpeg").c_str()));
    ASSERT_FALSE(std::remove((SaveConstants::pathToSave +
                             "/FramesetTest/5.jpeg").c_str()));
    ASSERT_FALSE(std::remove((SaveConstants::pathToSave +
                             "/FramesetTest").c_str()));
}
