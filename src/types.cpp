#include "types.hpp"

void printObject(const Object* value) {
  if (value == nullptr) {
    std::cout << "null object";
    return;
  }

  if (auto objString = dynamic_cast<const ObjString*>(value)) {
    std::cout << "\"" << objString->toString() << "\"";
  } else {
    std::cout << value->toString();
  }
}

void printValue(const Type& value) {
  std::visit(
      [](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, Null>) {
          std::cout << "null";
        } else if constexpr (std::is_same_v<T, bool>) {
          std::cout << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, Object*>) {
          printObject(arg);
        } else {
          std::cout << arg;
        }
      },
      value);
}

bool isObjType(const Type& value, const std::type_info& type) {
  if (isObject(value)) {
    Object* obj = asObject(value);
    if (obj) {
      return typeid(*obj) == type;
    }
  }
  return false;
}

ObjString* asString(const Type& value) {
  return dynamic_cast<ObjString*>(asObject(value));
}
