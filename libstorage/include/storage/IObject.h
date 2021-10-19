#pragma once

#ifndef STORAGE_IOBJECT_H
#define STORAGE_IOBJECT_H

#include <storage/Ipc.h>

#include <nlohmann/json.hpp>

#include <glog/logging.h>

#include <vector>

namespace tmk::storage {

using json = nlohmann::json;

enum class StorageType { NO_SAVE, SHORT_TERM, LONG_TERM };
constexpr  uint64_t IMPOSSIBLE_ID = 0;
constexpr  uint64_t MAX_NAME_LENGTH = 71;

struct ObjectID {
  char name[MAX_NAME_LENGTH + 1];
  uint64_t id = IMPOSSIBLE_ID;

  ObjectID() { name[0] = 0; }
  ObjectID(const std::string &name_) {
    if (name_.length() > MAX_NAME_LENGTH) {
      LOG(WARNING) << "ObjectID(): the length of string specified exceeds "
                      "maximal possible length "
                   << MAX_NAME_LENGTH << " (clipped)";
    }
    // data plus finishing zero
    name[name_.copy(name, MAX_NAME_LENGTH)] = 0;
  }
};

std::ostream &operator<<(std::ostream &out, const ObjectID &id);

using ObjectsIDs = std::vector<ObjectID>;

template <typename IdType> class IObject {
protected:
  IObject(const std::string &name, StorageType storageType_)
      : id(name), storageType(storageType_) {}

public:
  using IDType = IdType;

  json toJson() const {
    json ret;
    ret["id"] = id.id;
    ret["name"] = id.name;
    ret["storage_type"] = (int)storageType;
    return ret;
  }

  void fromJson(const json &obj) {
    if (!obj.contains("id") || !obj.contains("name") ||
        !obj.contains("storage_type")) {
      LOG(ERROR) << "JSON object does not contain IObject structure";
      return;
    }
    id = obj["name"].get<std::string>();
    id.id = obj["id"].get<uint64_t>();
    storageType = (StorageType)obj["storage_type"].get<int>();
  }

  IdType id;
  StorageType storageType;
};

} // namespace tmk::storage

namespace std {
template <> struct less<tmk::storage::ObjectID> {
  bool operator()(const tmk::storage::ObjectID &lhs,
                  const tmk::storage::ObjectID &rhs) const {
    return strcmp(lhs.name, rhs.name) < 0;
  }
};
} // namespace std

#endif // STORAGE_IOBJECT_H
