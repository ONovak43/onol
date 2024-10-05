#include "allocator.hpp"

#include <vector>
#include <cstdint>

enum class OpCode {
    OP_RETURN
};

class Bytecode {
private:
    std::vector<std::uint8_t, Allocator<std::uint8_t>> code;

public:
    Bytecode() = default;
    void put(OpCode byte);
    void free();
};
