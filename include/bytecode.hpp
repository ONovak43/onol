#include <cstdint>
#include <vector>

enum class OpCode {
    OP_RETURN
};

class Bytecode {
private:
    std::vector<uint8_t> code;

public:
    Bytecode() = default;
    void put(uint8_t byte);
};
