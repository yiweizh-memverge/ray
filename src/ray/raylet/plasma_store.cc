#include <functional>
#include <iostream>
#include <thread>

#include "gflags/gflags.h"
#include "ray/util/event.h"
#include "ray/object_manager/plasma/store_runner.h"
#include "ray/object_manager/common.h"

DEFINE_int32(plasma_store_port, 0, "The listen port the Plasma object store.");
DEFINE_string(cxl_controller_addr, "", "The Network address of the CXL controller.");
DEFINE_int32(cxl_controller_port, 0, "The Network port of the CXL controller.");
DEFINE_string(cxl_vendor, "", "The CXL device vendor.");
DEFINE_string(cxl_model, "", "The CXL device model.");
DEFINE_string(cxl_serial, "", "The CXL device serial number.");
DEFINE_int64(cxl_segment, 0, "The segment ofthe CXL device.");
DEFINE_string(log_dir, "/tmp/plasma_log", "The log directory.");

bool SpillObjectsCallback() {
  return false;
}

void ObjectStoreFullCallback() {
}

void AddObjectCallback(const ray::ObjectInfo & obj) {
}

void DeleteObjectCallback(const ray::ObjectInfo &) {
}

int main (int argc, char* argv[]) {
  InitShutdownRAII ray_log_shutdown_raii(ray::RayLog::StartRayLog,
                                         ray::RayLog::ShutDownRayLog,
                                         argv[0],
                                         ray::RayLogLevel::INFO,
                                         FLAGS_log_dir);
  ray::RayLog::InstallFailureSignalHandler(argv[0]);
  ray::RayLog::InstallTerminateHandler();

  gflags::ParseCommandLineFlags(&argc, &argv, true);

  std::string socket_name = std::string("0.0.0.0:") + std::to_string(FLAGS_plasma_store_port);
  plasma::plasma_store_runner.reset(
    new plasma::PlasmaStoreRunner(socket_name,
                                  8L * 1024 * 1024 * 1024,
                                  0,
                                  "/tmp/plasma",
                                  "/tmp/plasma",
                                  FLAGS_plasma_store_port,
                                  FLAGS_cxl_controller_addr,
                                  FLAGS_cxl_controller_port,
                                  FLAGS_cxl_vendor,
                                  FLAGS_cxl_model,
                                  FLAGS_cxl_serial,
                                  FLAGS_cxl_segment));


  auto store_thread = std::thread(&plasma::PlasmaStoreRunner::StartDefault,
                                  dynamic_cast<plasma::PlasmaStoreRunner*>(plasma::plasma_store_runner.get())
                                 ); 
  store_thread.join();
  return 0;
}
