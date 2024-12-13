#include "interned_strings.hpp"

ObjString* getOrIntern(String value) {
  static std::unordered_map<String, ObjString*> strings;

  auto it = strings.find(value);
  if (it != strings.end()) {
    return it->second;
  }

  ObjString* obj = allocateAndConstruct<ObjString>(std::move(value));
  strings[obj->value] = obj;
  return obj;
}

ObjString* getOrIntern(std::string_view value) {
  return getOrIntern(String(value.begin(), value.end(), Allocator<char>()));
}

ObjString* getOrIntern(const char* value) {
  return getOrIntern(String(value, value + std::strlen(value), Allocator<char>()));
}

void clearInternedStrings() {
  static std::unordered_map<String, ObjString*>& strings = []() -> auto& {
    static std::unordered_map<String, ObjString*> staticStrings;
    return staticStrings;
  }();

  for (auto& [key, value] : strings) {
    destructAndDeallocate(value);
  }
  strings.clear();
}
