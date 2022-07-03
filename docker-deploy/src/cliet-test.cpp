#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "client.hpp"
#include "inputParser.hpp"
int main(int argc, char ** argv) {
  std::vector<std::string> opts = {"hostname", "port"};
  InputParser parser;
  std::unordered_map<std::string, std::string> parsedOpt =
      parser.parseOpt<std::string>(argc, argv, opts);
  for (auto iter = parsedOpt.begin(); iter != parsedOpt.end(); iter++) {
    std::cout << iter->first << ": " << iter->second << std::endl;
  }

  Client client;
  std::string hostname = parsedOpt["hostname"];
  int port = std::stoi(parsedOpt["port"]);
  std::string request =
      "GET http://www.man7.org/linux/man-pages/man2/recv.2.html HTTP/1.1\n \
Host: www.man7.org\n \
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:95.0) Gecko/20100101 Firefox/95.0\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\n\
Accept-Language: zh-TW,zh;q=0.8,en-US;q=0.5,en;q=0.3\n\
Accept-Encoding: gzip, deflate\n\
Connection: keep-alive\n";
  char * recv = client.connectToHost(hostname, port, request);
  std::cout << recv << std::endl;
  delete[] recv;
}
