#include "compiler.hpp"

#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
using namespace std::string_literals;
#include <utility>
#include <vector>


void Pointer::move(bool toLeft) {
  if (toLeft == isOnLeft) {
    ++index;
  } else if (index == 0) {
    isOnLeft = !isOnLeft;
  } else {
    --index;
  }
}


bool Tape::checksize(const Pointer& pointer) {
  std::reference_wrapper<std::vector<cell_type>> correctTape{rightTape};
  if (pointer.isOnLeft) {
    correctTape = leftTape;
  }
  if (pointer.index >= correctTape.get().size()) {
    correctTape.get().resize(pointer.index + 1, 0);
    correctTape.get().resize(correctTape.get().capacity(), 0);
    return true;
  }
  return false;
}

void Tape::set(const Pointer& pointer, cell_type value) {
  if (pointer.isOnLeft) {
    leftTape[pointer.index] = value;
  } else {
    rightTape[pointer.index] = value;
  }
}

Tape::cell_type Tape::get(const Pointer& pointer) const {
  if (pointer.isOnLeft) {
    return leftTape[pointer.index];
  } else {
    return rightTape[pointer.index];
  }
}


void Interpreter::raiseError(size_t line, size_t column, const std::string& message) {
  throw std::runtime_error{(std::to_string(line) + ":"s + std::to_string(column) + message).c_str()};
}

void Interpreter::bindProgram(const std::string& code) {
  std::stack<size_t> bracketStack{};
  size_t pos{0};
  size_t column{1};
  size_t line{1};
  for (auto character : code) {
    switch (character) {
    case '[':
      bracketStack.push(pos);
      break;
    case ']':
      if (bracketStack.empty()) {
        raiseError(line, column, "Expected '[' before ']'.");
      }
      bracketMap[pos] = bracketStack.top();
      bracketMap[bracketStack.top()] = pos;
      bracketStack.pop();
      break;
    }
    switch (character) {
    case '+':
    case '-':
    case '>':
    case '<':
    case '[':
    case ']':
    case '.':
    case ',':
      ++pos;
      program.push_back(character);
      break;
    case '\n':
      ++line;
      column = 0;
      break;
    }
    ++column;
  }
  if (!bracketStack.empty()) {
    raiseError(line, column, "Expected ']' before EOF.");
  }
}

Interpreter::Interpreter(const std::string& code, std::ostream& out, std::istream& in):
  input{in},
  output{out} {
  bindProgram(code);
}

std::istream& Interpreter::getInput() {
  return input;
}

std::ostream& Interpreter::getOutput() {
  return output;
}

void Interpreter::writeInput(const std::string& value) {
  for (auto character : value) {
    input.putback(character);
  }
}

void Interpreter::reset() {
  counter = 0;
  tape = {};
  pointer = {};
}

bool Interpreter::unstop(bool stopOnEOF, std::set<size_t> stoppoints = {}) {
  while (counter < program.size()) {
    if (stoppoints.count(counter)) {
      return true;
    }
    char command{program[counter]};
    switch (command) {
    case '+':
    case '-':
      tape.set(pointer, tape.get(pointer) + (command == '+' ? 1 : -1));
      break;
    case '>':
    case '<':
      pointer.move(command == '<');
      tape.checksize(pointer);
      break;
    case '[':
    case ']':
      if ((tape.get(pointer) == 0) == (command == '[')) {
        counter = bracketMap[counter];
      }
      break;
    case '.':
      output << static_cast<char>(tape.get(pointer));
      output.flush();
      break;
    case ',':
      char tempbyte{};
      input >> tempbyte;
      if (input.eof()) {
        if (stopOnEOF) {
          return true;
        }
        break;
      } else if (input.fail()) {
        raiseError(0, 0, "Input stream is in fail state.");
      }
      tape.set(pointer, tempbyte);
    }
    ++counter;
  }
  return false;
}

bool Interpreter::run(bool stopOnEOF, std::set<size_t> stoppoints) {
  reset();
  return unstop(stopOnEOF, stoppoints);
}