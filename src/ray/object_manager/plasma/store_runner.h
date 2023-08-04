#pragma once

#include <boost/asio.hpp>
#include <memory>

#include "absl/synchronization/mutex.h"
#include "ray/common/asio/instrumented_io_context.h"
#include "ray/common/file_system_monitor.h"
#include "ray/object_manager/plasma/plasma_allocator.h"
#include "ray/object_manager/plasma/store.h"
#include "ray/object_manager/plasma/client.h"

namespace plasma {

class PlasmaStoreRunnerApi {
 public:
  virtual void Start(ray::SpillObjectsCallback spill_objects_callback,
                     std::function<void()> object_store_full_callback,
                     ray::AddObjectCallback add_object_callback,
                     ray::DeleteObjectCallback delete_object_callback) = 0;

  virtual void StartDefault() = 0;

  virtual void Stop() = 0;

  virtual bool IsPlasmaObjectSpillable(const ObjectID &object_id) = 0;

  virtual int64_t GetConsumedBytes() = 0;

  virtual int64_t GetFallbackAllocated() const = 0;

  virtual void GetAvailableMemoryAsync(std::function<void(size_t)> callback) const = 0; 
};

class PlasmaStoreRunnerDelegator : public PlasmaStoreRunnerApi{

 public:
  PlasmaStoreRunnerDelegator(const std::string& socket_name)
      : stop_(0), socket_path_(socket_name) {};

  void Start(ray::SpillObjectsCallback spill_objects_callback,
             std::function<void()> object_store_full_callback,
             ray::AddObjectCallback add_object_callback,
             ray::DeleteObjectCallback delete_object_callback);

  void StartDefault() {};

  void Stop() {
    stop_ = 1;
  }

  bool IsPlasmaObjectSpillable(const ObjectID &object_id);

  int64_t GetConsumedBytes();

  int64_t GetFallbackAllocated() const;

  void GetAvailableMemoryAsync(std::function<void(size_t)> callback) const;

 private:
   std::atomic_int stop_;
   const std::string socket_path_;
   ray::SpillObjectsCallback spill_callback_;
   std::function<void()> store_full_callback_;
   ray::AddObjectCallback add_object_callback_;
   ray::DeleteObjectCallback delete_object_callback_;
   std::shared_ptr<PlasmaEventClient> client_;
};

class PlasmaStoreRunner : public PlasmaStoreRunnerApi{
 public:
  PlasmaStoreRunner(std::string socket_name,
                    int64_t system_memory,
                    bool hugepages_enabled,
                    std::string plasma_directory,
                    std::string fallback_directory,
                    uint16_t port = 0,
                    const std::string& cx_controller = "",
                    uint16_t cxl_controller_port = 0,
                    const std::string& cxl_vendor = "",
                    const std::string& cxl_model = "",
                    const std::string& cxl_serial = "",
                    uint64_t cxl_segment = 0);
  void Start(ray::SpillObjectsCallback spill_objects_callback,
             std::function<void()> object_store_full_callback,
             ray::AddObjectCallback add_object_callback,
             ray::DeleteObjectCallback delete_object_callback);

  void StartDefault();

  void Stop();

  bool IsPlasmaObjectSpillable(const ObjectID &object_id);

  int64_t GetConsumedBytes();

  int64_t GetFallbackAllocated() const;

  void GetAvailableMemoryAsync(std::function<void(size_t)> callback) const {
    main_service_.post([this, callback]() { store_->GetAvailableMemory(callback); },
                       "PlasmaStoreRunner.GetAvailableMemory");
  }

 private:
  void Shutdown();
  mutable absl::Mutex store_runner_mutex_;
  std::string socket_name_;
  int64_t system_memory_;
  bool hugepages_enabled_;
  std::string plasma_directory_;
  std::string fallback_directory_;
  mutable instrumented_io_context main_service_;
  std::unique_ptr<IAllocator> allocator_;
  std::unique_ptr<ray::FileSystemMonitor> fs_monitor_;
  std::unique_ptr<PlasmaStore> store_;
  uint16_t listen_port_;
  CXLShmInfo cxl_shm_info_;
};

// We use a global variable for Plasma Store instance here because:
// 1) There is only one plasma store thread in Raylet.
// 2) The thirdparty dlmalloc library cannot be contained in a local variable,
//    so even we use a local variable for plasma store, it does not provide
//    better isolation.
extern std::unique_ptr<PlasmaStoreRunnerApi> plasma_store_runner;

}  // namespace plasma
