#include "bytecode.hpp"

#define DEBUG

int main() {
    Bytecode bytecode;
    bytecode.put(OpCode::OP_RETURN);
    bytecode.free();
    return 0;
}

