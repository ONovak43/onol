#include "interpreter_error.hpp"

#include <sstream>

InterpreterError::InterpreterError(uint32_t line, const std::string& msg)
    : line(line), message(msg) {
  std::ostringstream oss;
  oss << "[line " << line << "] Error: " << message;
  formattedMessage = oss.str();
}

const char* InterpreterError::what() const noexcept {
  return formattedMessage.c_str();
}

uint32_t InterpreterError::getLine() const noexcept {
  return line;
}
const std::string& InterpreterError::getMessage() const noexcept {
  return message;
}