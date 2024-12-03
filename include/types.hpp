#pragma once

#include <string>
#include <variant>

struct Null {};

using Type = std::variant<Null, int32_t, double, bool, std::string>;

void printValue(const Type& value);
