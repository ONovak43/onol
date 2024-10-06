#include "types.hpp"

void printValue(const Type& value) {
  std::visit([](auto&& arg) { std::cout << arg; }, value);
}
