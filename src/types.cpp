#include "types.hpp"

void printValue(const Type& value) {
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, Null>) {
            std::cout << "null";
        } else {
            std::cout << arg;
        }
    }, value);
}
