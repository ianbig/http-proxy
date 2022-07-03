#ifndef SERVER_HPP
#define SERVER_HPP

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <memory>
#include <string>

#include "network.hpp"

class Server {
  std::unique_ptr<Network> connect;

 public:
  Server();
  ~Server();
  int setupServer(const int port_num);
  void serverBoot(int socket_fd);
  int acceptRequest(int socket_fd);
};

std::string processRequest(int connection_fd);
#endif
