#pragma once
#include <set>
#include <mutex>
#include "ray/common/id.h"
namespace plasma {

class ObjectReferenceHistory {
 public:
  void AddObjectReference(const ray::ObjectID& id);

  bool RemoveObjectReference(const ray::ObjectID& id);

  static ObjectReferenceHistory& GetObjectReferenceManger() {
    return reference_manager_;
  }

 private:
  ObjectReferenceHistory() = default;
  std::mutex lock_;
  std::unordered_set<ray::ObjectID> referenced_objects_;
  static ObjectReferenceHistory reference_manager_; 
};

}
