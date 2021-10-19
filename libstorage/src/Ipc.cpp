#include <storage/Ipc.h>

#include <glog/logging.h>

namespace tmk::ipc {

std::tuple<boost::interprocess::managed_shared_memory, void_allocator>
createSharedMemory(const std::string &name, size_t memsize) {
  // Create shared memory segment
  boost::interprocess::managed_shared_memory segment{boost::interprocess::create_only, name.c_str(), memsize};
  // An allocator convertible to any allocator<T, segment_manager_t> type
  void_allocator shmemAlloc{segment.get_segment_manager()};
  return {std::move(segment), std::move(shmemAlloc)};
}

std::tuple<boost::interprocess::managed_shared_memory, void_allocator>
openSharedMemory(const std::string &name) {
  // Open shared memory segment for r/w access
  boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only, name.c_str()};
  // An allocator convertible to any allocator<T, segment_manager_t> type
  void_allocator shmemAlloc{segment.get_segment_manager()};
  return {std::move(segment), std::move(shmemAlloc)};
}

void destroySharedMemory(const std::string &name) {
  boost::interprocess::shared_memory_object::remove(name.c_str());
}

size_t availableSharedMemory(const std::string &name) {
  boost::interprocess::managed_shared_memory segment{boost::interprocess::open_only, name.c_str()};
  return segment.get_free_memory();
}

void growSharedMemory(const std::string &name, size_t growsize) {
  boost::interprocess::managed_shared_memory::grow(name.c_str(), growsize);
}

} // namespace tmk::ipc