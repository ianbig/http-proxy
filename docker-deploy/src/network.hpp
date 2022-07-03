#ifndef NETWORK_H
#define NETWORK_H

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <string>
#include <utility>

#include "httpParser.hpp"

#define MAX_MSG_LENGTH 700000
class Network {
  struct addrinfo * socketInfo;
  int socketfd;

 public:
  Network() : socketInfo(nullptr), socketfd(-1) {}

  template<typename T, typename U>
  std::pair<T, U> connectSetup(const char * hostName, int port_num) {
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = 0;
    if ((status = getaddrinfo(
             hostName, std::to_string(port_num).c_str(), &hints, &socketInfo)) != 0) {
      // TODO: throw exception
      fprintf(stderr, "Error: getaddrinfo error %s\n", gai_strerror(status));
      throw std::exception();
    }

    // TODO: refactor to loop through linked list
    if ((socketfd = socket(
             socketInfo->ai_family, socketInfo->ai_socktype, socketInfo->ai_protocol)) ==
        -1) {
      // TODO: throw exception
      perror("socket error");
      throw std::exception();
    }

    std::pair<T, U> connectInfo(socketfd, socketInfo);
    return connectInfo;
  }

  static void sendRequest(int client_connection_fd, const void * msg, const size_t size) {
    int num_bytes = 0;
    int recv_bytes = 0;
    while ((num_bytes < size)) {
      if ((recv_bytes = send(client_connection_fd, msg, size, 0)) == -1) {
        perror("send");
        throw std::exception();
      }

      num_bytes += recv_bytes;
    }
  }

  static std::string recvRequest(int connection_fd) {
    char buf[MAX_MSG_LENGTH] = {0};
    int num_bytes = 0;
    if ((num_bytes = recv(connection_fd, buf, sizeof(buf), 0)) == -1) {
      perror("recv");
      throw std::exception();
    }
    buf[num_bytes] = '\0';

    return std::string(buf);
  }

  static int recvConnectRequest(int connection_fd, char * recvBuf) {
    int num_bytes = 0;
    if ((num_bytes = recv(connection_fd, recvBuf, MAX_MSG_LENGTH, 0)) == -1) {
      perror("recv");
      throw std::exception();
    }
    recvBuf[num_bytes] = '\0';
    return num_bytes;
  }

  static int findContentLength(std::string s) {
    if (s.find("content-length") == std::string::npos) {
      return -1;
    }

    HttpParser parser;
    std::map<std::string, std::string> parsedHeader = parser.httpResMap(s);
    return std::stoi(parsedHeader["content-length"]);
  }

  static void assembleValidResponse(int socketfd,
                                    std::string & response,
                                    int contentLength) {
    if (response.find("\r\n\r\n") == std::string::npos) {
      throw std::exception();  // invalid input
    }

    while (response.size() < contentLength) {
      std::string tmp = recvRequest(socketfd);
      response.append(tmp);
    }
  }

  ~Network() {
    if (socketInfo != nullptr) {
      freeaddrinfo(socketInfo);
    }

    if (socketfd != -1) {
      if (close(socketfd) == -1) {
        // TODO: throw exception
        perror("~Network() close");
      }
    }
  }
};

#endif
