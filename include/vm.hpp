#pragma once

#include <deque>
#include <memory>
#include <variant>

#include "bytecode.hpp"
#include "interned_strings.hpp"
#include "parser.hpp"
#include "types.hpp"

enum class InterpretResult {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
};

class RuntimeError : public InterpreterError {
 public:
  RuntimeError() = delete;
  RuntimeError(uint32_t line, const std::string& msg)
      : InterpreterError(line, msg) {
  }
};

class VM {
 private:
  static constexpr size_t STACK_MAX = 256;
  std::shared_ptr<Bytecode> bytecode;
  uint8_t* ip;
  std::deque<Type> stack;
  Parser parser;
  std::vector<Object*, Allocator<Object*>> objects;
  std::unordered_map<String, Type> globals;

  Type pop();
  void push(Type value);
  Type peek(std::size_t distance) const;

  String checkVarExistsAndGetName(Type constName);

  std::size_t currentInstructionAddress();
  uint32_t getCurrentLine();
  String toString(const Type& value);

  template <typename T, typename... Args>
  T* allocateObject(Args&&... args) {
    T* obj = allocateAndConstruct<T>(std::forward<Args>(args)...);
    objects.push_back(obj);
    return obj;
  }

  template <typename Op>
  void binaryOp(Op op) {
    Type b = pop();
    Type a = pop();

    std::visit(
        [&](auto&& lhs, auto&& rhs) -> void {
          using LHS = std::decay_t<decltype(lhs)>;
          using RHS = std::decay_t<decltype(rhs)>;

          if constexpr ((std::is_same_v<LHS, int32_t> &&
                         std::is_same_v<RHS, int32_t>) ||
                        (std::is_same_v<LHS, double> &&
                         std::is_same_v<RHS, double>)) {
            push(op(lhs, rhs));
          } else if constexpr ((std::is_same_v<LHS, int32_t> &&
                                std::is_same_v<RHS, double>) ||
                               (std::is_same_v<LHS, double> &&
                                std::is_same_v<RHS, int32_t>)) {
            push(op(static_cast<double>(lhs), static_cast<double>(rhs)));
          } else if constexpr (std::is_same_v<LHS, Object*> ||
                               std::is_same_v<RHS, Object*>) {
            String lhsStr = toString(lhs);
            String rhsStr = toString(rhs);

            String result(lhsStr.begin(), lhsStr.end(), Allocator<char>());
            result.append(rhsStr.begin(), rhsStr.end());

            auto interned = getOrIntern(result);
            push(interned);
          } else {
            throw RuntimeError(bytecode->getLine(currentInstructionAddress()),
                               "Operator plus is not supported for this type.");
          }
        },
        a, b);
  }

  Type readConstantLong();

  inline uint8_t readByte() {
    return *ip++;
  }

  inline Type readConstant() {
    return bytecode->getConstant(readByte());
  }

 public:
  ~VM();
  InterpretResult interpret(const std::string& sourceCode);
  InterpretResult run();
};
