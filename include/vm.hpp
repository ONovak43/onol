#pragma once

#include <deque>
#include <memory>
#include <variant>

#include "bytecode.hpp"
#include "compiler.hpp"
#include "types.hpp"

enum class InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
};


class VM {
 private:
  static constexpr size_t STACK_MAX = 256;
  std::shared_ptr<Bytecode> bytecode;
  uint8_t* ip;
  std::deque<Type> stack;
  Compiler compiler;

  Type pop();
  void push(Type value);

  std::string toString(const Type& value) {
    return std::visit(
      [](auto&& arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
          return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
          return arg ? "true" : "false";
        } else {
          return std::to_string(arg);
        }
      },
      value);
  }

  template <typename Op>
  void binaryOp(Op op) {
    Type b = pop();
    Type a = pop();

    std::visit(
        [&](auto&& lhs, auto&& rhs) -> void {
          using LHS = std::decay_t<decltype(lhs)>;
          using RHS = std::decay_t<decltype(rhs)>;

          if constexpr ((std::is_same_v<LHS, int32_t> && std::is_same_v<RHS, int32_t>) ||
                        (std::is_same_v<LHS, double> && std::is_same_v<RHS, double>)) {
            push(op(lhs, rhs));
          } else if constexpr ((std::is_same_v<LHS, int32_t> && std::is_same_v<RHS, double>) ||
                               (std::is_same_v<LHS, double> && std::is_same_v<RHS, int32_t>)) {
            push(op(static_cast<double>(lhs), static_cast<double>(rhs)));
          } else if constexpr (std::is_same_v<LHS, std::string> || std::is_same_v<RHS, std::string>) {
            push(toString(lhs) + toString(rhs));
          }
        },
        a, b);
  }

  inline uint8_t readByte() {
    return *ip++;
  }

  inline Type readConstant() {
    return bytecode->getConstant(readByte());
  }

 public:
  InterpretResult interpret(const std::string& sourceCode);
  InterpretResult run();
};
