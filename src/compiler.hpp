#ifndef BFINTERPRETER3_COMPILER_HPP
#define BFINTERPRETER3_COMPILER_HPP

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

struct Pointer {
  size_t index{0};
  bool isOnLeft{false};

  void move(bool toLeft);
};

class Tape {
public:
  using cell_type = uint8_t;

  bool checksize(const Pointer& pointer);

  void set(const Pointer& pointer, cell_type value);
  cell_type get(const Pointer& pointer) const;

private:
  std::vector<cell_type> rightTape{0};
  std::vector<cell_type> leftTape{0};
};


class Interpreter {
public:
  Interpreter(const std::string& code, std::ostream& out, std::istream& in);

  std::istream& getInput();
  std::ostream& getOutput();

  void writeInput(const std::string& input);

  void reset();
  bool unstop(bool stopOnEOF, std::set<size_t> stoppoints);
  bool run(bool stopOnEOF, std::set<size_t> stoppoints = {});

private:
  void raiseError(size_t line, size_t collumn, const std::string& message);

  void bindProgram(const std::string& code);

  std::string program{""};
  size_t counter{0};
  std::map<size_t, size_t> bracketMap{};

  Tape tape{};
  Pointer pointer{};

  std::istream& input;
  std::ostream& output;
};




#endif