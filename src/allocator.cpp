#include "debug.hpp"

#include <allocator.hpp>
#include <cstdlib>

void *reallocate(void *pointer, [[maybe_unused]] size_t oldSize,
                 size_t newSize) {
  if (newSize == 0) {
    DEBUG_PRINT("[reallocate] Deallocating " + std::to_string(oldSize) +
                " bytes.");
    std::free(pointer);
    return nullptr;
  }

  return std::realloc(pointer, newSize);
}

template <typename T> T *Allocator<T>::allocate(std::size_t n) {
  if (n > std::size_t(-1) / sizeof(T)) {
    DEBUG_PRINT("[Allocator::allocate] Bad allocation.");
    throw std::bad_alloc();
  }
  return static_cast<T *>(reallocate(nullptr, 0, n * sizeof(T)));
}

template <typename T>
void Allocator<T>::deallocate(T *pointer, std::size_t n) noexcept {
  reallocate(static_cast<void *>(pointer), n * sizeof(T), 0);
}
