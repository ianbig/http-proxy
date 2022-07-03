#include "server.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "inputParser.hpp"
#include "network.hpp"

/**
 * Constructor for scoket
 */
Server::Server() : connect(new Network()) {
}

/**
 * Setup socket for the server on specified port number
 * @param port_num: the port for socket to bind on
 **/
int Server::setupServer(const int port_num) {
  std::pair<int, struct addrinfo *> socketInfo =
      connect.get()->connectSetup<int, struct addrinfo *>(NULL, port_num);

  int socket_fd = socketInfo.first;
  struct addrinfo * serviceinfo = socketInfo.second;

  int yes = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    // TODO: throw exception
    perror("setsockopt");
    throw std::exception();
  }

  if (bind(socket_fd, serviceinfo->ai_addr, serviceinfo->ai_addrlen) == -1) {
    // TODO: throw exception
    // perror("bind");
    throw std::exception();
  }

  if (listen(socket_fd, 100) == -1) {
    // TODO: refactor this
    perror("listen");
    throw std::exception();
  }

  return socket_fd;
}

/**
 * Bootup Server on a given socket
 * @param socket_fd: socket field number
 **/

void Server::serverBoot(int socket_fd) {
  while (1) {
    // accept
    int client_connection_fd = acceptRequest(socket_fd);
    // receive message
    std::string buf = Network::recvRequest(client_connection_fd);
    Network::sendRequest(client_connection_fd, buf.c_str(), buf.size());
  }
}

int Server::acceptRequest(int socket_fd) {
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  int client_connection_fd = 0;
  client_connection_fd =
      accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  if (client_connection_fd == -1) {
    perror("accept");
    throw std::exception();
  }

  return client_connection_fd;
}

/** 
 * Destructor for server class
 **/
Server::~Server() {
}
