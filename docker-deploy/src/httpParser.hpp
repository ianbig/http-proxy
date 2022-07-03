#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

void stringToLower(std::string & s);

class HttpParser {
  int getRequestType(std::string & response,
                     std::map<std::string, std::string> & responseMap);
  int getHeader(std::string & response, std::map<std::string, std::string> & responseMap);

 public:
  std::map<std::string, std::string> httpResMap(std::string header);
  std::string send200OK();
};
#endif
