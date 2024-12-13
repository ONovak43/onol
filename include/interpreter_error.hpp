#pragma once

#include <exception>
#include <string>
#include <cstdint>

class InterpreterError : public std::exception {
 private:
  uint32_t line;
  std::string message;
  std::string formattedMessage; 

 public:
  InterpreterError() = delete;
  InterpreterError(uint32_t line, const std::string& msg);
  virtual ~InterpreterError() = default;
  virtual const char* what() const noexcept override;
  uint32_t getLine() const noexcept;
  const std::string& getMessage() const noexcept;
};