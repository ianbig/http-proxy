#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <memory>
#include <string>
#include <vector>

#include "network.hpp"

class Client {
  std::unique_ptr<Network> cconnect;

 public:
  Client() : cconnect(new Network) {}
  int setUpSocket(std::string hostname, int port_num);
  std::string connectToHost(std::string hostname,
                            int port,
                            std::string request,
                            int socket_fd);
};
#endif
