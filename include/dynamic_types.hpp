#pragma once

#include <cstring>
#include <memory>
#include <string>

#include "allocator.hpp"

struct Object {
  virtual ~Object() = default;
  virtual std::string toString() const = 0;
  virtual bool operator==(const Object& other) const = 0;
};

using String = std::basic_string<char, std::char_traits<char>, Allocator<char>>;

struct ObjString : Object {
  String value;

  ObjString(String value) : value(std::move(value)) {
  }

  ObjString(const std::string& str) : value(str.begin(), str.end()) {
  }

  ObjString(std::string_view sv) : value(sv.begin(), sv.end()) {
  }

  ObjString(const char* cstr) : value(cstr, cstr + std::strlen(cstr)) {
  }

  ~ObjString() {
    std::cout << "ObjStringdestructor" << "\n";
  }

  std::string toString() const override {
    return std::string(value.begin(), value.end());
  }

  const char* toChar() const {
    return value.c_str();
  }

  bool operator==(const Object& other) const override {
    if (auto* otherString = dynamic_cast<const ObjString*>(&other)) {
      return value == otherString->value;
    }
    return false;
  }
};

bool areEqual(const Object* lhs, const Object* rhs);
