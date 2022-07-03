#ifndef INPUTPAPRSER_HPP
#define INPUTPAPRSER_HPP
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
class InputParser {
 public:
  template<typename U>
  std::unordered_map<std::string, U> parseOpt(int argc,
                                              char ** argv,
                                              std::vector<std::string> opts) {
    if (argc < 2) {
      // TODO: refactor this to throw exception
      std::cerr << "Error: wrong command format" << std::endl;
      throw std::exception();
    }

    std::unordered_map<std::string, U> parsedOpt;
    for (int i = 1; i < argc; i++) {
      std::stringstream ss;
      ss << argv[i];
      U tmp;
      ss >> tmp;

      if (ss.fail()) {
        // TODO: throw exception
        fprintf(stderr, "Exception: invalid converion for parameter %s\n", argv[i]);
        throw std::exception();
      }

      std::string option =
          opts[i - 1];  // argv corresponding value is 1 more that parsedOpt
      parsedOpt[option] = tmp;
    }

    return parsedOpt;
  }
};
#endif
