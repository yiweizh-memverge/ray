#include "ray/object_manager/plasma/cxl_allocator.h"

namespace plasma {

CXLShmAllocator::CXLShmAllocator(uint64_t footprint,
                                 const std::string& controller_addr,
                                 uint16_t controller_port,
                                 const std::string& client_name,
                                 const std::string& client_addr,
                                 const std::string& cxl_vendor,
                                 const std::string& cxl_model,
                                 const std::string& cxl_serial,
                                 uint64_t cxl_segment) : footprint_limit_(footprint) {
  std::shared_ptr<memverge::cxl::shared_memory::CXLSharedMemoryClient> client;
  int ret = memverge::cxl::shared_memory::CXLSharedMemoryClient::ConnectServer(
      controller_addr,
      controller_port,
      client_name,
      client_addr,
      &client);
  if (ret) {
    RAY_LOG(FATAL) << "Failed to create CXL shared memory client";
    return;
  }
  memverge::cxl::shared_memory::CXLDeviceInfo dev;
  dev.vendor = cxl_vendor;
  dev.model = cxl_model;
  dev.serial = cxl_serial;
  ret = client->AttachCXLSegment(dev, cxl_segment, true, &cxl_memory_);
  if (ret) {
    RAY_LOG(FATAL) << "Failed to Attach CXL segment: " << ret
                   << ":" << cxl_vendor
                   << ":" << cxl_model
                   << ":" << cxl_serial << ":" << cxl_segment;
  } else {
    footprint_limit_ = cxl_memory_->Size();
  }
}

absl::optional<Allocation> CXLShmAllocator::Allocate(size_t bytes) {
  if (!cxl_memory_) return absl::nullopt;
  void* ptr = cxl_memory_->AlignedAllocateMemory(64, bytes);
  if (!ptr) return absl::nullopt;
  MEMFD_TYPE fd;
  fd.first = CXL_SHM_FD;
  fd.second = CXL_SHM_ID;
  allocated_ += bytes;
  auto offset = reinterpret_cast<uintptr_t>(ptr) - cxl_memory_->BaseAddr();
  return Allocation(ptr, static_cast<int64_t>(bytes), fd, offset, 0, cxl_memory_->Size(), false);
}

absl::optional<Allocation> CXLShmAllocator::FallbackAllocate(size_t bytes) {
  return Allocate(bytes);
}

void CXLShmAllocator::Free(Allocation allocation) {
  if (!cxl_memory_) return;
  cxl_memory_->FreeMemory(allocation.address);
  allocated_ -= allocation.size;
}


};

