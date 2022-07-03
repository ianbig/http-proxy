#include "server.hpp"

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "inputParser.hpp"

int main(int argc, char ** argv) {
  std::vector<std::string> opts = {"port"};
  InputParser parser;
  std::unordered_map<std::string, int> parsedOpt = parser.parseOpt<int>(argc, argv, opts);

  Server server;
  int socket_fd = server.setupServer(parsedOpt["port"]);
  server.serverBoot(socket_fd);
}
