#ifndef PROXY_HPP
#define PROXY_HPP

#include "CacheController.hpp"
#include "HttpLog.hpp"
#include "client.hpp"
#include "server.hpp"

void sendToHost(std::string hostname, int port, int socketfd, std::string request);

// TODO: move this to Network class
void handleGetRequest(std::string hostname,
                      int port,
                      int socketfd,
                      std::string http_request,
                      std::map<std::string, std::string> & headerMap,
                      CacheController * cache,
                      HttpLog & logger);
// TODO: move this to Network class
void handleConnectRequest(std::string hostname,
                          int port,
                          int client_fd,
                          std::string http_request,
                          HttpLog & logger,
                          std::map<std::string, std::string> & headerMap);
// TODO: move this to Network class
void setupConnectIO(struct pollfd pfds[], int client_fd, int server_fd);
void handlePostRequest(std::string hostname,
                       int port,
                       int client_fd,
                       std::string http_request);
void handleNewTab(int client_connection_fd,
                  CacheController * cache,
                  HttpLog & logger,
                  int id);
std::string getIP(std::string hostname);

class Proxy : public Server {
  CacheController * cache;
  HttpLog logger;

 public:
  static int requestId;
  Proxy() : cache(new CacheController(5)) {}
  Proxy(size_t size) : cache(new CacheController(size)) {}
  ~Proxy();
  int proxyServerSetup(int port);
  void serverBoot(int socketfd);
  void dispatch_worker(int socketfd, CacheController * cache, int id);
};

int Proxy::requestId = 0;

#endif
