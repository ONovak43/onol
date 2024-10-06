#pragma once

#include <cstddef>

void *reallocate(void *pointer, [[maybe_unused]] size_t oldSize, size_t newSize);

template <typename T> struct Allocator {
  using value_type = T;

  Allocator() = default;

  T *allocate(std::size_t n);
  void deallocate(T *p, std::size_t) noexcept;
};

template <typename T, typename U>
bool operator==(const Allocator<T> &, const Allocator<U> &) {
  return true;
}

template <typename T, typename U>
bool operator!=(const Allocator<T> &a, const Allocator<U> &b) {
    return !(a == b);
}

template <typename T>
T *Allocator<T>::allocate(std::size_t n) {
  if (n > std::size_t(-1) / sizeof(T)) {
    throw std::bad_alloc();
  }
  return static_cast<T *>(reallocate(nullptr, 0, n * sizeof(T)));
}

template <typename T>
void Allocator<T>::deallocate(T *pointer, std::size_t n) noexcept {
  reallocate(static_cast<void *>(pointer), n * sizeof(T), 0);
}
