#include <misc/name_generator.h>

#include <storage/storage_interface.h>

#include <storage/Ipc.h>
#include <storage/ObjectDescriptor.h>

#include <stdio.h>

#include <iostream>

using namespace tmk;
using namespace tmk::storage;

extern "C" size_t storage_interface_init(char* segment_name, size_t size){
    //TODO: think about NULL
    // Create boost shared memory
    ipc::createSharedMemory(segment_name, size);
    if (setenv(BOOST_SEGMENT_NAME_ENV, segment_name, 1) != 0){
        fprintf(stderr, "[storage_interface]: can't set %s value for child process: errno=%d, %s\n", BOOST_SEGMENT_NAME_ENV, errno, strerror(errno));
        return -1;
    }
    return ipc::availableSharedMemory(segment_name);
}

extern "C" size_t storage_interface_new_object(char* segment_name, char *obj_name) {
    // Connect to shared as client and create object with name
    auto [segment, clientShmemAlloc] = ipc::openSharedMemory(segment_name);
    ipc::createShared<ObjectDescriptor>(
        segment, obj_name, StorageType::NO_SAVE, ObservableType::PIPE);
    return ipc::availableSharedMemory(segment_name);
}

extern "C" size_t storage_interface_get_object(void **obj_ptr, char* segment_name, char *obj_name) {
    // Connect to shared as client and create object with name
    auto [segment, clientShmemAlloc] = ipc::openSharedMemory(segment_name);
    auto obj = ipc::findShared<ObjectDescriptor>(segment, obj_name);
    *obj_ptr = (void*)(&obj);
    return ipc::availableSharedMemory(segment_name);
}

extern "C" size_t storage_interface_new_object_with_framesets(char* segment_name, char *obj_name, int source_number) {
    // Connect to shared as client and create object with name
    auto [segment, shmemAlloc] = ipc::openSharedMemory(segment_name);
    //TODO: not good place for ObservableType
    ObjectRef<ObjectDescriptor> object = ipc::createShared<ObjectDescriptor>(segment, obj_name, StorageType::NO_SAVE, ObservableType::PIPE);
    //std::cout << "Available after create object: " << ipc::availableSharedMemory(segment_name) << std::endl;
    // create framesets
    //StorageType st = StorageType::NO_SAVE;
    char postfix[SHMEM_NAME_LEN], fsname[SHMEM_NAME_LEN];
    get_object_postfix(obj_name, postfix);
    for (int source_id = 0; source_id < source_number; source_id++) {
        generate_name_with_id(fsname, (char*)("frameset"), source_id, postfix);
        //fprintf(stdout, "[storage_interface]: frameset %s planning\n", fsname);
        //fflush(stdout);
#if 0
        try {
            FramesetRef frameset = object().addFrameset(shmemAlloc, fsname, StorageType::NO_SAVE);
            //std::cout << "Available after create named frameset#" << source_id << ": " << ipc::availableSharedMemory(segment_name) << std::endl;
        }
        catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
        ipc::destroyShared<Frameset>(segment, fsname);
        //std::cout << "Available after destroy named frameset#" << source_id << ": " << ipc::availableSharedMemory(segment_name) << std::endl;
#else
        object().addFrameset(shmemAlloc, fsname, StorageType::NO_SAVE); 
        //ipc::destroyShared<Frameset>(segment, fsname);
        //std::cout << "Available after destroy named frameset#" << source_id << ": " << ipc::availableSharedMemory(segment_name) << std::endl;
#endif
    }
    return ipc::availableSharedMemory(segment_name);
}

namespace cv {
    class Mat;
};

extern "C"  size_t storage_interface_add_frame_to_object_with_framesets(char *segment_name, char *obj_name, int source_id, int frame_id, void *img_ptr) {
    auto [segment, clientShmemAlloc] = ipc::openSharedMemory(segment_name);
    auto clientObj = ipc::findShared<ObjectDescriptor>(segment, obj_name);

    auto framesetID = clientObj().framesetsIDs()[source_id];
    FramesetRef frameset = clientObj().frameset(framesetID);

    TimeT tstamp = system_clock::now();
    int sequenceNum = frame_id;

    StorageType st = StorageType::NO_SAVE;
    char postfix[SHMEM_NAME_LEN], frame_name[SHMEM_NAME_LEN];
    generate_dt_postfix(postfix);
    //get_object_postfix(obj_name, postfix);
    generate_name_with_src_and_id(frame_name, (char*)("frame"), source_id, frame_id, postfix);
#if 0
    try {
        cv::Mat *img = (cv::Mat *)img_ptr;
        FrameRef frame = frameset().addFrame(clientShmemAlloc, frame_name, st, *img, tstamp, sequenceNum, source_id);
        ipc::destroyShared<Frame>(segment, frame_name);
    }
    catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
#else
    frameset().addFrame(clientShmemAlloc, frame_name, st, *(cv::Mat *)img_ptr, tstamp, sequenceNum, source_id);
    ipc::destroyShared<Frame>(segment, frame_name);
#endif
    return ipc::availableSharedMemory(segment_name);
}

void analyze_frame(FrameRef &frame) {
    auto img = frame().image();
    int imgSize = img.total() * img.elemSize();
    std::cout<< "frame[" << frame().id << "] imgSize: " << imgSize << std::endl;
    fflush(stdout);
}

void analyze_frameset(FramesetRef &fs) {
    int number_frames = fs().framesCount();
    std::cout << "number frames in framesets[" << fs().id << "]: " << number_frames << std::endl;
    fflush(stdout);
#if 0
    int count = 0;
    for(auto &frameID : fs().framesIDs())
    {
        std::cout<< "frame" << "["<< count <<"].name: "<< frameID.name << std::endl;
        std::cout<< "frame" << "["<< count <<"].id: "  << frameID.id << std::endl;
        auto frame = fs().frame(frameID);
        analyze_frame(frame);
        ++count;
    }
#else
    if (number_frames > 0) {
        auto frame = fs().frame(fs().framesIDs()[0]);
        analyze_frame(frame);
    }
#endif
}

void analyze_object(ObjectRef<ObjectDescriptor> &object) {
    std::cout << "number framesets in object: " << object().framesetsCount() << std::endl;
    auto framesets = object().framesetsIDs();
    int count = 0;
    for(auto &framesetID : framesets)
    {
        std::cout<< "frameset" << "["<< count <<"].name: "<< framesetID.name << std::endl;
        std::cout<< "frameset" << "["<< count <<"].id:"  << framesetID.id << std::endl;
        auto fs = object().frameset(framesetID);
        analyze_frameset(fs);
        ++count;
    }
}

extern "C" void storage_interface_analyze_object_with_framesets(char *segment_name, char *obj_name) {
    auto [segment, clientShmemAlloc] = ipc::openSharedMemory(segment_name);
    auto clientObj = ipc::findShared<ObjectDescriptor>(segment, obj_name);
    analyze_object(clientObj);
    fflush(stdout);
}

extern "C" size_t storage_interface_free_object(char* segment_name, char *obj_name) {
    // Connect to shared as client and destroy object by name
    auto [segment, shmemAlloc] = ipc::openSharedMemory(segment_name);
    ObjectRef<ObjectDescriptor> object = ipc::findShared<ObjectDescriptor>(segment, obj_name);
    if (object) { 
#if 0
        auto framesets = object().framesetsIDs();
        for(auto &framesetID : framesets) {
           auto fs = object().frameset(framesetID);
           //TODO: free of object without this action is correct, but I want to know how should work this place
           for(auto &frameID : fs().framesIDs()) {
                auto frame = fs().frame(frameID);
                ipc::destroyShared<Frame>(segment, frame);
            }
            ipc::destroyShared<Frameset>(segment, fs);
            //std::cout << "Available after destroying frameset:" << i << "/" << fsc <<": " << ipc::availableSharedMemory("SharedObjects") << std::endl;
        }
        ipc::destroyShared<ObjectDescriptor>(segment, object);
        std::cout << "Available after free memory: " << ipc::availableSharedMemory(segment_name) << std::endl;
#else
        ipc::destroyShared<ObjectDescriptor>(segment, object);
        std::cout << "Available after free memory: " << ipc::availableSharedMemory(segment_name) << std::endl; 
#endif
    }
    return ipc::availableSharedMemory(segment_name);
}

extern "C"  void storage_interface_destroy(char* segment_name) {
#if 0
    struct shm_remove {
        shm_remove() { ipc::destroySharedMemory(segment_name); }
        ~shm_remove() { ipc::destroySharedMemory(segment_name); }
    } remover;
#else
    #include <sys/mman.h>
    shm_unlink(segment_name);
#endif
}

