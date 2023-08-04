#include "ray/object_manager/plasma/object_reference_manager.h"
namespace plasma {

ObjectReferenceHistory ObjectReferenceHistory::reference_manager_;

void ObjectReferenceHistory::AddObjectReference(const ray::ObjectID& id) {
  std::lock_guard<std::mutex> lg(lock_);
  referenced_objects_.insert(id);
}

bool ObjectReferenceHistory::RemoveObjectReference(const ray::ObjectID& id) {
  std::lock_guard<std::mutex> lg(lock_);
  return referenced_objects_.erase(id) > 0;
}

}
