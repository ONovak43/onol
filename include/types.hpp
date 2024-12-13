#pragma once

#include <variant>

#include "dynamic_types.hpp"

struct Null {
  friend bool operator==(const Null&, const Null&) {
    return true;
  }
};


using Type = std::variant<Null, int32_t, double, bool, Object*>; // TODO check Object maybe causes memory leak (for containers)

void printValue(const Type& value);
bool isObjType(const Type& value, const std::type_info& type);

inline bool isNull(const Type& value) {
  return std::holds_alternative<Null>(value);
}

inline bool isInt(const Type& value) {
  return std::holds_alternative<int32_t>(value);
}

inline bool isDouble(const Type& value) {
  return std::holds_alternative<double>(value);
}

inline bool isBool(const Type& value) {
  return std::holds_alternative<bool>(value);
}

inline bool isObject(const Type& value) {
  return std::holds_alternative<Object*>(value);
}

inline Object* asObject(const Type& value) {
  return std::get<Object*>(value);
}

inline bool isString(const Type& value) {
  return isObjType(value, typeid(ObjString));
}

inline int32_t asInt(const Type& value) {
  return std::get<int32_t>(value);
}

inline double asDouble(const Type& value) {
  return std::get<double>(value);
}

inline bool asBool(const Type& value) {
  return std::get<bool>(value);
}

inline bool isSameType(const Type& value1, const Type& value2) {
  return value1.index() == value2.index();
}

ObjString* asString(const Type& value);