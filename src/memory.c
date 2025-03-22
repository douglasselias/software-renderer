#include <stdlib.h>

void* mem_alloc(int size) {
  return calloc(size, 1);
}

void mem_reset(void* data, size_t size) {
  memset(data, 0, size);
}