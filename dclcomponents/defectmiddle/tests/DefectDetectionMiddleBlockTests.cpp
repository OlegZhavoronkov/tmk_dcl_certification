#include "DefectMiddle.h"

#include "dclprocessor/dclprocessor.h"
#include "tcpcapture/cvcollector.h"
#include <storage/ObjectDescriptor.h>
#include <storage/ObjectRef.h>
#include <storage/Ipc.h>

#include <torch/torch.h>
#include <torch/script.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "gtest/gtest.h"

using namespace tmk;
using namespace tmk::ipc;

namespace DefectDetectionMiddleConstants {
// or "/home/alin/test/TMK-DCL2/dclcomponents/defectmiddle/data/"
const std::string prefix =  "../dclcomponents/defectmiddle/data/";
const std::string test_path = prefix + "traced_linear_mnist.pt";
// ~320kB
const std::string img = prefix + "image.jpg";
// ~60-120kB each; 390kB total
const std::string img1 = prefix + "defects/segmap_22_IMG_9964214_crop.jpg";
const std::string img2 = prefix + "defects/segmap_34_IMG_9964217_crop.jpg";
const std::string img3 = prefix + "defects/segmap_42_IMG_9964219_crop.jpg";
const std::string img4 = prefix + "defects/segmap_58_IMG_996424_crop.jpg";
const std::string img5 = prefix + "defects/segmap_62_IMG_996425_crop.jpg";
} // namespace DefectDetectionMiddleConstants

TEST(MiddleBlockTest, CheckTorchScriptLoad) {
    torch::jit::load(DefectDetectionMiddleConstants::test_path);
}

TEST(MiddleBlockTest, CheckNeuronNet) {
    cv::Mat image = cv::imread(DefectDetectionMiddleConstants::img, 0);

    DefectDetectionNetworkV2 model;
    model.load(DefectDetectionMiddleConstants::TorchScriptModulePath);
    auto mask = model.getDefectMask(image);
    LOG(INFO) << "[test]: Got mask, containing "
                      << countNonZero(mask) << " non-zero values" << std::endl;

    struct shm_remove {
       shm_remove() {
           ipc::destroySharedMemory("TestDefect");
       }
       ~shm_remove() {
           ipc::destroySharedMemory("TestDefect");
       }
    } remover;

    auto [managed_shm, shmemAlloc] =
      tmk::ipc::createSharedMemory("TestDefect", 3000000);
    auto myObj = ipc::createShared<ObjectDescriptor>(managed_shm,
                                                     "aaa",
                                                     StorageType::NO_SAVE,
                                                     ObservableType::PIPE);
    auto frameset = myObj().addFrameset(shmemAlloc, "Frameset",
                                        StorageType::NO_SAVE);
    frameset().addFrame(shmemAlloc, "1", StorageType::NO_SAVE,
                        image, system_clock::now(), 100, 5);
    DefectModel(myObj, (char *)"boost_path", (char *)"aaa");
    /* should be 0 */
}

TEST(MiddleBlockTest, DefectDetectionTest) {
    cv::Mat image1 = cv::imread(DefectDetectionMiddleConstants::img1);
    cv::Mat image2 = cv::imread(DefectDetectionMiddleConstants::img2);
    cv::Mat image3 = cv::imread(DefectDetectionMiddleConstants::img3);
    cv::Mat image4 = cv::imread(DefectDetectionMiddleConstants::img4);
    cv::Mat image5 = cv::imread(DefectDetectionMiddleConstants::img5);

    struct shm_remove {
       shm_remove() {
           ipc::destroySharedMemory("TestDefect");
       }
       ~shm_remove() {
           ipc::destroySharedMemory("TestDefect");
       }
    } remover;

    auto [managed_shm, shmemAlloc] =
      tmk::ipc::createSharedMemory("TestDefect", 13000000);
    auto myObj = ipc::createShared<ObjectDescriptor>(managed_shm,
                                                     "aaa",
                                                     StorageType::NO_SAVE,
                                                     ObservableType::PIPE);
    auto frameset = myObj().addFrameset(shmemAlloc, "Frameset",
                                        StorageType::NO_SAVE);
    frameset().addFrame(shmemAlloc, "1", StorageType::NO_SAVE,
                        image1, std::chrono::system_clock::now(), 100, 5);
    frameset().addFrame(shmemAlloc, "2", StorageType::NO_SAVE,
                        image2, std::chrono::system_clock::now(), 100, 2);
    frameset().addFrame(shmemAlloc, "3", StorageType::NO_SAVE,
                        image3, std::chrono::system_clock::now(), 100, 5);
    frameset().addFrame(shmemAlloc, "4", StorageType::NO_SAVE,
                        image4, std::chrono::system_clock::now(), 100, 5);
    frameset().addFrame(shmemAlloc, "5", StorageType::NO_SAVE,
                        image5, std::chrono::system_clock::now(), 100, 2);
    DefectModel(myObj, (char *)"boost_path", (char *)"aaa");
    /* 44332  14347  62900  15108  26252 */
}

void EmptyWork(ObjectDescriptor &obj, char *boost_path, char *obj_name) {
    (void)obj;
    LOG(INFO) << "[empty]: Hello, I'm object " << obj_name
              << " from " << boost_path << std::endl;
}

TEST(MiddleBlockTest, ModuleTest) {
    const char *expected = "Ok";
    const char *request = "Ok";
    ASSERT_STREQ(request, expected);
    
    // TODO: implement test here?
    /*struct shm_remove {
       shm_remove() {
           ipc::destroySharedMemory("TestDefect");
       }
       ~shm_remove() {
           ipc::destroySharedMemory("TestDefect");
       }
    } remover;

    auto [managed_shm, shmemAlloc] =
      tmk::ipc::createSharedMemory("TestDefect", 6400000);
    putenv("SHMEM_DATA_NAME=TestDefect"); // for pipeline's getenv()
    putenv("SHMEM_DATA_SIZE=6400000");
    putenv("SHMEM_PROCESS_ID=${pidof defectmiddle-tests}"); // TODO: ...check?*/
    ////pipeline_first_process("localhost", "9999", EmptyWork);
    ////pipeline_middle_process(DefectModel);
    //struct data_collector *collector = NULL;
    //collector = collector_init("localhost", "9999");
    //if (!collector) {
    //    LOG(ERROR) << "Collector couldn't init, may be camera isn't ready. "
    //             << "Please, try again or work without stream. " << std::endl;
    //}
    //connect_to_pipeline(PC_FIRST, (void*)collector, EmptyWork);
    //if (collector) collector_destroy(collector);
    //connect_to_pipeline(PC_MIDDLE, NULL, DefectModel);
}

