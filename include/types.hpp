#pragma once

#include <string>
#include <variant>

using Type = std::variant<int32_t, double, bool, std::string>;

void printValue(const Type& value);
