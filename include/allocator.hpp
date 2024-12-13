#pragma once

#include <cstddef>
#include <memory>

void *reallocate(void *pointer, [[maybe_unused]] size_t oldSize,
                 size_t newSize);

template <typename T>
struct Allocator {
  using value_type = T;

  Allocator() = default;

  template <typename U>
  Allocator(const Allocator<U> &) noexcept {
  }

  T *allocate(std::size_t n);
  void deallocate(T *p, std::size_t) noexcept;

  template <typename... Args>
  T *create(Args &&...args) {
    T *ptr = allocate(1);
    new (ptr) T(std::forward<Args>(args)...);
    return ptr;
  }

  void destroyPtr(T *ptr) {
    if (ptr) {
      ptr->~T();
      deallocate(ptr, 1);
    }
  }
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

template <typename T, typename... Args>
T *allocateAndConstruct(Args &&...args) {
  Allocator<T> alloc;
  return alloc.create(std::forward<Args>(args)...);
}

template <typename T>
void destructAndDeallocate(T *ptr) {
  Allocator<T> alloc;
  alloc.destroyPtr(ptr);
}