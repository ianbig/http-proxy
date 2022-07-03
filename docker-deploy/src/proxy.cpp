#include "proxy.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>

#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <thread>

#include "CacheController.hpp"
#include "HttpLog.hpp"
#include "httpParser.hpp"
#include "inputParser.hpp"
#include "network.hpp"
#define DEFAULT_PORT 12345

int Proxy::proxyServerSetup(int port) {
  logger.openLogFile("/var/log/erss/proxy.log");  // TODO: write this to /var/err/*.log
  return setupServer(port);
}

void Proxy::serverBoot(int socketfd) {
  int id = 0;
  while (1) {
    int client_connection_fd = acceptRequest(socketfd);
    dispatch_worker(client_connection_fd, cache, id++);
  }
}

void handleNewTab(int client_connection_fd,
                  CacheController * cache,
                  HttpLog & logger,
                  int id) {
  try {
    while (1) {
      // get header from client
      std::string http_request = Network::recvRequest(client_connection_fd);

      // parse header
      HttpParser httpParser;
      std::map<std::string, std::string> headerMap = httpParser.httpResMap(http_request);
      std::string hostname = headerMap["host"];
      int port = (headerMap["Port"] == "-1" || headerMap["Port"] == "")
                     ? 80
                     : std::stoi(headerMap["Port"]);
      std::string method = headerMap["Method"];
      headerMap["id"] = std::to_string(Proxy::requestId++);

      // handle action
      if (method.find("GET") != std::string::npos) {
        handleGetRequest(
            hostname, port, client_connection_fd, http_request, headerMap, cache, logger);
      }

      else if (method.find("CONNECT") != std::string::npos) {
        handleConnectRequest(
            hostname, port, client_connection_fd, http_request, logger, headerMap);
      }

      else if (method.find("POST") != std::string::npos) {
        handlePostRequest(hostname, port, client_connection_fd, http_request);
      }
    }
  }

  catch (std::exception & e) {
    handleNewTab(client_connection_fd, cache, logger, id);
  }
}

void handleConnectRequest(std::string hostname,
                          int port,
                          int client_fd,
                          std::string http_request,
                          HttpLog & logger,
                          std::map<std::string, std::string> & requestMap) {
  // setup connection with server
  Client c;
  int server_fd = c.setUpSocket(hostname, port);
  // send back 200 ok with client
  HttpParser httpParser;
  std::string responseOk = httpParser.send200OK();
  Network::sendRequest(client_fd, "HTTP/1.1 200 OK\r\n\r\n", 19);

  // setup IO mux
  struct pollfd pfds[2];  // only client and server
  int fd_size = 2;
  setupConnectIO(pfds, client_fd, server_fd);
  HttpParser parser;

  // Use Poll IO to listen to socket that is ready for recving response
  while (1) {
    int poll_count = poll(pfds, fd_size, -1);
    for (int i = 0; i < fd_size; i++) {
      if (pfds[i].revents & POLLIN) {
        char recvBuf[MAX_MSG_LENGTH] = {0};
        int num_bytes = Network::recvConnectRequest(pfds[i].fd, recvBuf);
        int sendIndex = fd_size - i - 1;
        Network::sendRequest(pfds[sendIndex].fd, recvBuf, num_bytes);

        std::string logMsg =
            requestMap["id"] + ": \"CONNECT " + requestMap["host"] + "/ HTTP/1.1\" from ";
        logger.writeLog(logMsg);

        std::string serverMsg = requestMap["id"] + ": Requesting \"Connect " +
                                requestMap["host"] + "/ HTTP/1.1\" from " +
                                requestMap["host"];
        logger.writeLog(serverMsg);
        std::string clientMsg = requestMap["id"] + ": Responding \"HTTP/1.1 200 OK\"";
        logger.writeLog(clientMsg);
        std::string endMsg = requestMap["id"] + ": Tunnel closed";
        logger.writeLog(endMsg);
      }
    }
  }
}

Proxy::~Proxy() {
  delete cache;
}

void handlePostRequest(std::string hostname,
                       int port,
                       int client_fd,
                       std::string http_request) {
  Client c;
  std::string recvbuf;
  int server_fd = c.setUpSocket(hostname, port);
  recvbuf = c.connectToHost(hostname, port, http_request, server_fd);
  Network::sendRequest(client_fd, recvbuf.c_str(), recvbuf.size());
}

void handleGetRequest(std::string hostname,
                      int port,
                      int client_fd,
                      std::string http_request,
                      std::map<std::string, std::string> & requestMap,
                      CacheController * cache,
                      HttpLog & logger) {
  std::string recvbuf;
  std::map<std::string, std::string> responseMap;
  HttpParser parser;

  if (cache->toRevalidate(requestMap["url"], logger, requestMap["id"]) == true) {
    // get resposne from server
    Client c;
    int server_fd = c.setUpSocket(hostname, port);

    recvbuf = c.connectToHost(hostname, port, http_request, server_fd);
    responseMap = parser.httpResMap(recvbuf);
    // insert response to cache
    responseMap["Body"] = recvbuf;
    cache->putInCache(requestMap["url"], responseMap);
  }

  else {
    recvbuf = cache->getCache(requestMap["url"]);
    responseMap = parser.httpResMap(recvbuf);
  }

  std::string logMsg = requestMap["id"] + ": \"GET " + requestMap["host"] +
                       "/ HTTP/1.1\" from " + getIP(requestMap["host"]) + " @ " +
                       responseMap["date"];
  logger.writeLog(logMsg);

  std::string serverMsg = requestMap["id"] + ": Requesting \"GET " + requestMap["host"] +
                          "/ HTTP/1.1\" from " + requestMap["host"];
  logger.writeLog(serverMsg);

  std::string responseMsg = requestMap["id"] + ": Received \"HTTP/1.1 " +
                            responseMap["StatusCode"] + "\" from " + requestMap["host"];

  logger.writeLog(responseMsg);
  std::string clientMsg =
      requestMap["id"] + ": Responding \"HTTP/1.1 " + responseMap["StatusCode"] + "\"";
  logger.writeLog(clientMsg);
  Network::sendRequest(client_fd, recvbuf.c_str(), recvbuf.size());
  std::string endMsg = requestMap["id"] + ": Tunnel closed";
  logger.writeLog(endMsg);
}

std::string getIP(std::string hostname) {
  hostent * record = gethostbyname(hostname.c_str());
  in_addr * address = (in_addr *)record->h_addr;
  std::string ip_address = inet_ntoa(*address);

  return ip_address;
}

void Proxy::dispatch_worker(int socketfd, CacheController * cache, int id) {
  std::thread(handleNewTab, socketfd, std::ref(cache), std::ref(logger), id).detach();
}

// TODO: move this to Network class
void setupConnectIO(struct pollfd pfds[], int client_fd, int server_fd) {
  pfds[0].fd = client_fd;
  pfds[0].events = POLLIN;
  pfds[1].fd = server_fd;
  pfds[1].events = POLLIN;
}

int main(int argc, char ** argv) {
  Proxy p;
  int socketfd = p.proxyServerSetup(DEFAULT_PORT);
  p.serverBoot(socketfd);
}
