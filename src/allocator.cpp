#include "allocator.hpp"
#include <cstdlib>

void *reallocate(void *pointer, [[maybe_unused]] size_t oldSize, size_t newSize) {
  if (newSize == 0) {
    std::free(pointer);
    return nullptr;
  }

  return std::realloc(pointer, newSize);
}
