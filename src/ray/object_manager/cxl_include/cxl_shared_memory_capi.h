#ifndef CXL_SHARED_MEMORY_CPAI_H
#define CXL_SHARED_MEMORY_CPAI_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" { 
#endif

struct cxl_shared_memory_segment {
  uint64_t size;
  uint64_t map_addr;
  uint64_t base_addr;
  bool (*is_locked)(struct cxl_shared_memory_segment*);
  void (*release_lock)(struct cxl_shared_memory_segment*);
  bool (*is_writable)(struct cxl_shared_memory_segment*);
  bool (*acquire_lock)(struct cxl_shared_memory_segment*, bool writable);
  uint64_t (*allocated_size)(struct cxl_shared_memory_segment*);
  void* (*allocate_memory)(struct cxl_shared_memory_segment*, size_t);
  void* (*aligned_allocate_memory)(struct cxl_shared_memory_segment*, size_t, size_t);
  void (*free_memory)(struct cxl_shared_memory_segment*, void*);
  void* (*reallocate_memory)(struct cxl_shared_memory_segment*, void*, size_t);
  void (*full_memory_barrier)(struct cxl_shared_memory_segment*, bool full);
  void (*memory_barrier)(struct cxl_shared_memory_segment*, void*, size_t);
  int (*read_rpc_data)(struct cxl_shared_memory_segment*, void* buf, size_t, size_t);
  int (*write_rpc_data)(struct cxl_shared_memory_segment*, void* buf, size_t, size_t);
  void (*commit_rpc)(struct cxl_shared_memory_segment*);
  void (*register_rpc_callback)(struct cxl_shared_memory_segment*, void (*cb)(struct cxl_shared_memory_segment*, void*), void*);
  void (*destroy_cxl_shared_memory_segment)(struct cxl_shared_memory_segment*); 
};

struct cxl_shared_memory_client {
  const char* server_addr;
  uint16_t server_port;
  const char* client_name;
  const char* client_addr;
  struct cxl_shared_memory_segment* (*attach_cxl_shared_memory_segment)(
      struct cxl_shared_memory_client*, const char* vendor,
      const char* model, const char* serial, const char* type, uint64_t offset, bool writable); 
  
  void (*destroy_cxl_shared_memory_client)(struct cxl_shared_memory_client*);
};

struct cxl_shared_memory_client* create_cxl_shared_memory_client(
    const char* server, uint16_t port, const char* client, const char* addr);


#ifdef __cplusplus
}
#endif
#endif
