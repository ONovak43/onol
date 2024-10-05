#pragma once

#include <cstddef>
#include <cstdint>

void* reallocate(void* pointer, size_t oldSize, size_t newSize);

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

template struct Allocator<uint8_t>;
