#ifndef DBPG_DESCRIPTORSDB_H
#define DBPG_DESCRIPTORSDB_H

#include <dbpg/Filesystem.h>
#include <storage/ObjectDescriptor.h>

#include <iostream>
#include <string_view>

namespace tmk::dbpg {

using json = nlohmann::json;

class DescriptorsDB {
public:
  using SharedMemory =
      std::tuple<ipc::managed_shared_memory, ipc::void_allocator>;

  DescriptorsDB(const std::string &shmemName_, const fs::path &dbfsRootDir_)
      : dbfsRootDir(dbfsRootDir_), shmemName(shmemName_),
        sharedMemory(ipc::openSharedMemory(shmemName)) {}

  // saves/updates shared memory object to DB
  // void update(const storage::ObjectDescriptorID &id);
  // checks and reloads object if necessary
  // void refresh(const storage::ObjectDescriptorID &id);

  std::vector<storage::ObjectDescriptorID> objectIds(); // get available objects
  storage::ObjectDescriptorRef load(const storage::ObjectDescriptorID &id);
  bool save(const storage::ObjectDescriptorID &id);

  void shortTermCleanup();

  void removeFromSharedMemory(const storage::ObjectDescriptorID &id);

  SharedMemory
      sharedMemory; // TODO:: consider an accessor method and make private
private:
  fs::path dbfsRootDir;
  std::string shmemName;
  static constexpr std::string_view desctiptorJsonFilename = "descriptor.json";
};

} // namespace tmk::dbpg
#endif // DBPG_DESCRIPTORSDB_H