#include <unordered_map>

#include "allocator.hpp"
#include "dynamic_types.hpp"

#pragma once

#include <unordered_map>
#include "allocator.hpp"
#include "types.hpp"

ObjString* getOrIntern(String value);
ObjString* getOrIntern(std::string_view value);
ObjString* getOrIntern(const char* value);
void clearInternedStrings();
