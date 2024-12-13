#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>

#include "allocator.hpp"
#include "bytecode.hpp"

TEST_CASE("Bytecode basic operations", "[bytecode]") {
  SECTION("An instruction is added to the bytecode with the correct line") {
    Bytecode bytecode;
    OpCode instruction = OpCode::RETURN;

    bytecode.putOpCode(instruction, 10);

    REQUIRE(bytecode.count() == 1);
    REQUIRE(bytecode.getOpCode(0) == OpCode::RETURN);
    REQUIRE(bytecode.getLine(0) == 10);
  }

  SECTION("When Bytecode is freed, it becomes empty") {
    Bytecode bytecode;
    OpCode instruction = OpCode::RETURN;
    bytecode.putOpCode(instruction, 1);

    bytecode.free();

    REQUIRE(bytecode.count() == 0);
  }
}

TEST_CASE("Bytecode getLine operations", "[bytecode]") {
  SECTION("Getting correct line for a single instruction") {
    Bytecode bytecode;
    OpCode instruction = OpCode::RETURN;

    bytecode.putOpCode(instruction, 5);

    REQUIRE(bytecode.getLine(0) == 5);
  }

  SECTION("Getting correct line for multiple instructions") {
    Bytecode bytecode;

    bytecode.putOpCode(OpCode::RETURN, 5);
    bytecode.putOpCode(OpCode::RETURN, 5);
    bytecode.putOpCode(OpCode::RETURN, 10);
    bytecode.putOpCode(OpCode::RETURN, 15);

    REQUIRE(bytecode.getLine(0) == 5);
    REQUIRE(bytecode.getLine(1) == 5);
    REQUIRE(bytecode.getLine(2) == 10);
    REQUIRE(bytecode.getLine(3) == 15);
  }
}

TEST_CASE("Bytecode constant operations", "[bytecode]") {
  SECTION("Added constant can be retrieved") {
    Bytecode bytecode;
    Type value = 10;
    uint32_t line = 10;

    std::size_t address = bytecode.putConstant(value, line);

    auto storedVal = bytecode.getConstant(address);
    REQUIRE(storedVal == value);
  }

  SECTION("Adding multiple constants to the bytecode and verifying their addresses") {
    Bytecode bytecode;
    Type valueInt = 10;
    Type valueStr = allocateAndConstruct<ObjString>("test");
    uint32_t line1 = 10;
    uint32_t line2 = 20;

    std::size_t addressInt = bytecode.putConstant(valueInt, line1);
    std::size_t addressStr = bytecode.putConstant(valueStr, line2);

    auto storedInt = bytecode.getConstant(addressInt);
    REQUIRE(storedInt == valueInt);

    auto storedStr = bytecode.getConstant(addressStr);
    REQUIRE(storedStr == valueStr);
  }
}

TEST_CASE("Bytecode instruction tests", "[bytecode]") {
  SECTION("Adding CONSTANT and CONSTANT_LONG instructions") {
    Bytecode bytecode;
    Type valueInt = 10;
    Type valueStr = allocateAndConstruct<ObjString>("test");
    uint32_t line1 = 10;
    uint32_t line2 = 20;

    std::size_t address1 = bytecode.putConstant(valueInt, line1);
    std::size_t address2 = bytecode.putConstant(valueStr, line2);

    REQUIRE(bytecode.getOpCode(0) == OpCode::CONSTANT);
    REQUIRE(bytecode.getConstantAddress(1) == address1);
    REQUIRE(bytecode.getLine(0) == line1);

    for (int i = 0; i < 300; i++) {
      bytecode.putConstant(110, line2);
    }
    std::size_t addressLong = bytecode.putConstant(valueStr, line2);
    REQUIRE(bytecode.getOpCode(696) == OpCode::CONSTANT_LONG);
    CHECK(bytecode.getLine(696) == line2);
  }
}
