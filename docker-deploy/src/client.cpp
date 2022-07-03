#include "client.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "httpParser.hpp"
#include "network.hpp"

/** Connect to specified host
 * @param hostname: hostname to connect
 * @param port_num: port number
 * @param http_request
 * @return char * need to free by programmer
 **/
std::string Client::connectToHost(std::string hostname,
                                  int port_num,
                                  std::string http_request,
                                  int socket_fd) {
  Network::sendRequest(socket_fd, http_request.c_str(), http_request.size());

  std::string response = Network::recvRequest(socket_fd);
  std::string validResponse = response;
  // find the end of header
  while (validResponse.find("\r\n\r\n") == std::string::npos) {
    std::string tmp = Network::recvRequest(socket_fd);
    validResponse.append(tmp);
  }

  HttpParser parser;
  std::map<std::string, std::string> parsed = parser.httpResMap(validResponse);
  int contentLength = (parsed.find("content-length") != parsed.end())
                          ? std::stoi(parsed["content-length"])
                          : -1;
  if (contentLength == -1) {
    throw std::exception();
  }

  // start to parse data
  Network::assembleValidResponse(socket_fd, validResponse, contentLength);

  return validResponse;
}

int Client::setUpSocket(std::string hostname, int port_num) {
  Network * client_connect = cconnect.get();
  std::pair<int, struct addrinfo *> socketInfo =
      client_connect->connectSetup<int, struct addrinfo *>(hostname.c_str(), port_num);

  int socket_fd = socketInfo.first;
  struct addrinfo * serviceinfo = socketInfo.second;

  if (connect(socket_fd, serviceinfo->ai_addr, serviceinfo->ai_addrlen) == -1) {
    // TOOO: refactor to throw exception
    perror("connect");
    throw std::exception();
  }

  return socket_fd;
}
