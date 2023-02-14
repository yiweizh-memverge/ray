#pragma once
#include "ray/object_manager/plasma/allocator.h"
#include "ray/object_manager/plasma/common.h"
#include "ray/object_manager/cxl_include/cxl_shared_memory_client.h"

namespace plasma {

class CXLShmAllocator : public IAllocator {
 public:

  CXLShmAllocator(uint64_t footprint_limit,
                  const std::string& controller_addr,
                  uint16_t controller_port,
                  const std::string& client_name,
                  const std::string& client_addr,
                  const std::string& cxl_vendor,
                  const std::string& cxl_model,
                  const std::string& cxl_serial,
                  uint64_t segment);

  ~CXLShmAllocator() = default;

  absl::optional<Allocation> Allocate(size_t bytes);

  absl::optional<Allocation> FallbackAllocate(size_t bytes);

  void Free(Allocation allocation);

  int64_t GetFootprintLimit() const { return footprint_limit_; };

  int64_t Allocated() const { return allocated_; };

  int64_t FallbackAllocated() const { return fallback_allocated_; };

 private:

  std::shared_ptr<memverge::cxl::shared_memory::CXLSharedSegmentInf> cxl_memory_;
  uint64_t allocated_ = 0;
  uint64_t fallback_allocated_ = 0;
  uint64_t footprint_limit_;
};

};

