#pragma once

#include <cstdint>

class VM {
    private:
        Chunk* chunk;
        uint8_t* ip;
};
