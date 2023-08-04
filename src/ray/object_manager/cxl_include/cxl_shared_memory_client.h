/*
 *  * Copyright (C) 2022 MemVerge Inc.
 *   */
/* This file defines the major APIs of the CXL shared library */
#ifndef CXL_SHARED_MEMORY_CLIENT_H
#define CXL_SHARED_MEMORY_CLIENT_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "cxl_shared_memory_datatype.h"

namespace memverge::cxl::shared_memory {

class CXLSharedSegmentInf : public std::enable_shared_from_this<CXLSharedSegmentInf> {
 public:

  virtual ~CXLSharedSegmentInf() {};

  virtual void Detach() = 0;

  /* Acquire a readlock */
  virtual int AcquireReadLock() = 0;

  /* Acquire the write lock */
  virtual int AcquireWriteLock(int timeout) = 0;

  /* Check if the lock is held*/
  virtual int IsLocked() const = 0;

  /* Release the lock */
  virtual int ReleaseLock() = 0;

  /* Check if the segment memory is writable*/
  virtual bool IsWritable() const = 0;

  /* The virtual address of the usable memory */
  virtual uintptr_t BaseMapAddr() const = 0;

  /* The virutla address of the segment is mapped */
  virtual uintptr_t BaseAddr() const = 0;

  /* Usage size of the segment */
  virtual uint64_t Size() const = 0;

  /* The allocated size */
  virtual uint64_t AllocatedSize() const = 0;

  /* Allocate memory from the segment with the given size */
  virtual void* AllocateMemory(size_t size) = 0;

  /* Allocate memory from the segment with the given size and alignment*/
  virtual void* AlignedAllocateMemory(size_t align, size_t size) = 0;

  /* Free allocaed memory*/
  virtual void FreeMemory(void* ptr) = 0;

  /* Reallocate the memory pointed by ptr*/
  virtual void* ReallocateMemory(void* ptr, size_t size) = 0;

  /* Flush CPU cache for all the used regions from the segment*/
  virtual void MemBarrier(bool full = false) = 0;

  /* Flush CPU cache for the specified region.
     ptr is the virtual address */
  virtual void MemBarrier(void* ptr, size_t size) = 0;

  virtual void DoNTMemCpy(void* dest, const void* src, size_t size) = 0;

  /* The reserved RPC buffer */
  virtual uint64_t RPCBuffSize() const = 0;

  /* Update the RPC buffer and wake up the partner*/
  virtual void CommitRPC() = 0;

  /* Read data from the RPC buffer */
  virtual int ReadRPCData(void* buf, size_t size, size_t offset) = 0;

  /* Write data to the RPC buffer */
  virtual int WriteRPCData(void* buf, size_t size, size_t offset) = 0;

  /* Register the RPC callback. This will start a thread to poll memory*/
  virtual int RegisterRPCCallback(std::function<void(CXLSharedSegmentInf*, void*)>, void* p) = 0;

  /* Get the backend CXL device */
  virtual const std::string& DevicePath() const = 0; 
};

class CXLSharedMemoryClient : public std::enable_shared_from_this<CXLSharedMemoryClient> {
 public:
  /* Connect to the shared memory controller. The client_name must be unique.
     The client_addr is not used yet.
     On success, it creates a CXLSharedMemoryClient instance*/
  static int ConnectServer(const std::string& addr, uint16_t port,
                           const std::string& client_name,
                           const std::string& client_addr,
                           std::shared_ptr<CXLSharedMemoryClient>* result);

  virtual ~CXLSharedMemoryClient() {};

  /* Retrieve all the CXL devices*/
  virtual int ListCXLDevices(std::vector<CXLDeviceInfo>*) = 0;

  /*Attach a CXL shared memory segment.
   dev: CXL memory device identity
   offset: The semgnet offset on device. It is the segment ID
   writable: attach the segment as read only or read write.
   On success, it creates the CXLSharedSegmentInf instance
  */
  virtual int AttachCXLSegment(const CXLDeviceInfo& dev,
                               uint64_t offset,
                               bool writable,
                               std::shared_ptr<CXLSharedSegmentInf>*) = 0;

  virtual int AddCXLDevice(const CXLDeviceInfo&) = 0;

  virtual int DeleteCXLDevice(const CXLDeviceInfo&) = 0;

  /* Create a new CXL shared segment.
     size: segment size in bytes
     multi_write: allow multiple writers to the segment
  */
  virtual int CreateCXLDeviceSegment(const CXLDeviceInfo& device,
                                     uint64_t size,
                                     bool multi_write) = 0;

  virtual int DeleteCXLDeviceSegment(const CXLDeviceInfo&, uint64_t) = 0;

  virtual int ListCXLClients(std::vector<CXLClientInfo>*) = 0;
};
  
} // namespace
#endif
