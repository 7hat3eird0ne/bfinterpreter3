#include "compiler.hpp"

#include <fstream>
#include <iostream>
#include <optional>

int main(int argc, char* argv[]) {
  std::ifstream inputFile;
  std::ofstream outputFile;
  bool streamsDefined{false};
  if (argc == 1) {
    std::cerr << "Not sufficient amount of arguments. (Arguments: (path) SOURCEFILE, [STOPMARK], [OUTPUTFILE, INPUTFILE])\n";
    return EXIT_FAILURE;
  } else if (argc == 4) {
    std::cerr << "Input file not provided.\n";
    return EXIT_FAILURE;
  } else if (argc == 5) {
    streamsDefined = true;
    outputFile = std::ofstream{argv[3]};
    inputFile = std::ifstream{argv[4]};
    if (!inputFile) {
      std::cerr << "Input file does not exist. (Input filepaths come after output filepaths)\n";
    }
  } else if (!(argc == 2 || argc == 3 || argc > 5)) {
    std::cerr << "Too many arguments.\n";
    return EXIT_FAILURE;
  }
  unsigned char stopmark;
  if (argc > 2) {
    stopmark = argv[2][0];
  }

  std::ifstream targetfile{argv[1]};
  if (!targetfile) {
    std::cerr << "File not found.\n";
  }
  std::string sourceCode{};
  while (targetfile) {
    std::string line{};
    std::getline(targetfile, line);
    sourceCode.append(line);
  }
  std::optional<Interpreter> programWrapper;
  try {
    programWrapper.emplace(sourceCode, (streamsDefined ? outputFile : std::cout), (streamsDefined ? inputFile : std::cin), stopmark);
  } catch (std::runtime_error& except) {
    std::cerr << except.what() << "\n";
    return EXIT_FAILURE;
  }
  Interpreter program{*programWrapper};
  try {
    if (program.run(false)) {
      std::cout << "Input EOF or a stop mark has been hit.\n";
    }
    return EXIT_SUCCESS;
  } catch (std::runtime_error& except) {
    std::cerr << except.what() << "\n";
    return EXIT_FAILURE;
  }
}