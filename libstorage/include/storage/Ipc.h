#pragma once

#ifndef STORAGE_IPC_H
#define STORAGE_IPC_H

#include <storage/ObjectRef.h>
#include <storage/IpcTypes.h>

#include <glog/logging.h>

namespace tmk::ipc {

// Create shared memory segment
std::tuple<boost::interprocess::managed_shared_memory, void_allocator>
createSharedMemory(const std::string &name, size_t memsize);

// Open shared memory segment for r/w access
std::tuple<boost::interprocess::managed_shared_memory, void_allocator>
openSharedMemory(const std::string &name);

// Destroy shared memory segment and free system memory
void destroySharedMemory(const std::string &name);

// Get size of free space in shared memory segment
size_t availableSharedMemory(const std::string &name);

// Grow shared memory,
// no process should be mapped/modifying the shared memory while the growing
// is performed
void growSharedMemory(const std::string &name, size_t growsize);

/**
 * @brief Create named object (Frame, Frameset or ObjectDescriptor) in shared
 * memory segment. User is responsible to use destroyShared(name) even after
 * moving this object.
 * for example:
 * myObjectDescriptor.addFrameset(std::move(myCreatedFrameset))
 * destroyShared(segment, myCreatedFrameset);
 */
template <typename T, typename... Args>
static storage::ObjectRef<T> createShared(boost::interprocess::managed_shared_memory &segment,
                                          const std::string &name,
                                          Args &&...args) {
  storage::ObjectRef<T> ref;
  try {
    void_allocator shmemAlloc(segment.get_segment_manager());
    T *ptr = segment.construct<T>(("shmobject_" + name).c_str())(
        shmemAlloc, name, std::forward<Args>(args)...);
    if (ptr != nullptr) {
      ref = *ptr;
    }
  } catch (const std::exception &e) {
    LOG(ERROR) << typeid(e).name() << e.what();
  }
  if (!ref.valid()) {
    LOG(ERROR) << "No shared memory available for object \"" << name << "\"";
  } else {
    LOG(INFO) << "Shared object " << typeid(T).name() << "{\"" << name
              << "\"} created";
  }
  return ref;
}

// Get access to named object in shared memory segment
template <typename T>
static storage::ObjectRef<T> findShared(boost::interprocess::managed_shared_memory &segment,
                                        const std::string &name) {
  storage::ObjectRef<T> ref;
  auto [ptr, count] = segment.find<T>(("shmobject_" + name).c_str());
  if (ptr != nullptr) {
    ref = *ptr;
    LOG(INFO) << "Shared object " << typeid(T).name() << "{\"" << name
              << "\"} found";
  }
  if (!ref.valid()) {
    LOG(ERROR) << "No shared memory object \"" << name << "\" found ";
  }
  return ref;
}

// Delete named object from shared memory segment
template <typename T>
static void destroyShared(boost::interprocess::managed_shared_memory &segment,
                          const std::string &name) {
  segment.destroy<T>(("shmobject_" + name).c_str());
  LOG(INFO) << "Shared object " << typeid(T).name() << "{\"" << name
            << "\"} destroyed";
}

// Delete object from shared memory segment
// !!!before!!! storage::ObjectRef<T> obj !!!after!!! storage::ObjectRef<T>& obj
template <typename T>
static void destroyShared(boost::interprocess::managed_shared_memory &segment,
                          storage::ObjectRef<T>& obj) {
  auto id = obj().id;

  segment.destroy_ptr<T>(&obj());
  // ==== last edit 20.04.21 ===
  std::cout<<"### delete obj.id: "<< id << std::endl;
  obj.reset();
  // ==== last edit 20.04.21 ===

  LOG(INFO) << "Shared object " << typeid(T).name() << "{\"" << id
            << "\"} destroyed";
}

} // namespace tmk::ipc

#endif // STORAGE_IPC_H