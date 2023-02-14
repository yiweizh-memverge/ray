/*
 *  *  * Copyright (C) 2022 MemVerge Inc.
 *   *   */
/* This file defines the major APIs of the CXL shared library */

#ifndef CXL_SHARED_MEMORY_DATA_TYPE_H
#define CXL_SHARED_MEMORY_DATA_TYPE_H

#include <string>
#include <vector>

namespace memverge::cxl::shared_memory {

/* One CXL shared memory segment */
struct CXLSegmentInfo {
  /* Offset on CXL device in byte*/
  uint64_t dev_offset;
  /* Size of the segment, in byte */
  uint64_t segment_size;
  /* The address that the segment will be mapped on*/
  uint64_t map_addr;
  int lock_status = 0;
  /* The write owner of the segment */
  std::string write_owner;
  /* How many processes are using the segment */
  int user_count = 0;
  CXLSegmentInfo() = default;

  CXLSegmentInfo(uint64_t offset, uint64_t size, uint64_t addr)
      : dev_offset(offset),
        segment_size(size),
        map_addr(addr) {}
};

/* The CXL device information */
struct CXLDeviceInfo {
  /* The device vendor */
  std::string vendor;
  /* The device model */
  std::string model;
  /* The device serial number */
  std::string serial;
  /* Type of the device */
  std::string type;
  /* The device size in byte*/
  uint64_t size;
  /* The virtual address that the devicre will be mapped */
  uint64_t base_addr;
  /* All the segments on the device */
  std::vector<CXLSegmentInfo> segments;
  std::string ID() const {
    return vendor + "_" + model + "_" + serial;
  }
  friend bool operator==(const CXLDeviceInfo& info, const CXLDeviceInfo& dev) {
    if (dev.vendor == info.vendor && dev.model == info.model
        && dev.serial == info.serial)
      return true;
    return false;
  }
};

struct CXLClientSegment {
  CXLDeviceInfo dev;
  CXLSegmentInfo segment;
};

/* The process that uses CXL shared memory is a client.
 * This defines the client status */
struct CXLClientInfo {
  std::string client_name;
  std::string client_addr;
  std::vector<CXLClientSegment> segments;
};

} // namespace

#endif
