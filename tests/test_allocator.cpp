#include <catch2/catch_all.hpp>
#include <cstdint>
#include <vector>

#include "allocator.hpp"

TEST_CASE("Allocator basic allocation", "[allocator]") {
  Allocator<uint8_t> allocator;

  SECTION("Allocate and deallocate single int") {
    uint8_t* p = allocator.allocate(1);
    REQUIRE(p != nullptr);

    allocator.deallocate(p, 1);
    SUCCEED("Deallocated single int without crashing");
  }

  SECTION("Allocate and deallocate array of ints") {
    uint8_t* p = allocator.allocate(10);
    REQUIRE(p != nullptr);

    allocator.deallocate(p, 10);
    SUCCEED("Deallocated array of ints without crashing");
  }
}

TEST_CASE("Allocator with std::vector", "[allocator]") {
  SECTION("Allocates memory that is used with std::vector") {
    std::vector<uint8_t, Allocator<uint8_t>> vec;

    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    REQUIRE(vec.size() == 3);
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 2);
    REQUIRE(vec[2] == 3);
  }
}

TEST_CASE("Reallocate function", "[reallocate]") {
  SECTION("Allocate memory using reallocate") {
    void* p = reallocate(nullptr, 0, 100);

    REQUIRE(p != nullptr);
    reallocate(p, 100, 0);
    SUCCEED("Allocated and deallocated memory successfully");
  }

  SECTION("Reallocate to larger size") {
    void* p = reallocate(nullptr, 0, 50);
    REQUIRE(p != nullptr);

    void* p_larger = reallocate(p, 50, 150);
    REQUIRE(p_larger != nullptr);

    reallocate(p_larger, 150, 0);
    SUCCEED("Reallocated to larger size and deallocated successfully");
  }

  SECTION("Reallocate to smaller size") {
    void* p = reallocate(nullptr, 0, 200);
    REQUIRE(p != nullptr);

    void* p_smaller = reallocate(p, 200, 100);
    REQUIRE(p_smaller != nullptr);

    reallocate(p_smaller, 100, 0);
    SUCCEED("Reallocated to smaller size and deallocated successfully");
  }

  SECTION("Reallocate with zero size") {
    void* p = reallocate(nullptr, 0, 100);
    REQUIRE(p != nullptr);

    void* p_deallocated = reallocate(p, 100, 0);
    REQUIRE(p_deallocated == nullptr);
    SUCCEED("Deallocated memory successfully using reallocate with size 0");
  }
}
