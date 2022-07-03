#include "httpParser.hpp"

std::string HttpParser::send200OK() {
  std::string okString = "HTTP/1.1 200 OK\r\n\r\n";
  return okString;
}

/** 
 * get type of the request
 * @param response: http request or http response
 * @return 0 for success
 **/
int HttpParser::getRequestType(std::string & response,
                               std::map<std::string, std::string> & responseMap) {
  size_t pos = 0;
  std::string firstLine;
  std::string delimiter = "\r\n";
  response.find_first_not_of(delimiter);
  pos = response.find(delimiter);
  // find first line to check if it's response or request
  firstLine = response.substr(0, pos);
  // response HTTP/ will be at the begining of the first line
  int firstLine_pos = firstLine.find("HTTP/");
  int first_space_pos = firstLine.find(" ");
  if (firstLine_pos - first_space_pos > 5) {  //http://
    responseMap.insert(
        {"url",
         firstLine.substr(first_space_pos + 1, firstLine_pos - first_space_pos - 2)});
  }
  if (firstLine_pos == 0) {
    responseMap.insert({"Type", "Response"});
    firstLine_pos = firstLine.find(" ");
    responseMap.insert(
        {"StatusCode", firstLine.substr(firstLine_pos + 1, firstLine.length())});
  }
  else if (firstLine_pos > 0) {
    responseMap.insert({"Type", "Request"});
    firstLine_pos = firstLine.find(" ");
    responseMap.insert({"Method", firstLine.substr(0, firstLine_pos + 1)});
  }

  response.erase(0, pos + delimiter.length());

  return 0;
}

int HttpParser::getHeader(std::string & response,
                          std::map<std::string, std::string> & responseMap) {
  int pos = response.find("\r\n\r\n");
  std::string header = response.substr(0, pos);
  response.erase(0, pos);

  std::string headerDeli = ":";
  std::string delimiter = "\n";
  while ((pos = header.find(headerDeli)) != std::string::npos) {
    std::string tokenKey = header.substr(0, pos);
    // cout << tokenKey << endl;
    header.erase(0, pos + headerDeli.length());
    pos = header.find(delimiter);
    std::string tokenValue = header.substr(1, pos - 1);
    // cout << tokenValue << endl;
    header.erase(0, pos + delimiter.length());
    stringToLower(tokenKey);
    responseMap.insert({tokenKey, tokenValue});
  }

  return 0;
}

/**
 * Turn the string of http request or repsone to key-value pair map
**/
std::map<std::string, std::string> HttpParser::httpResMap(std::string response) {
  std::map<std::string, std::string> responseMap;

  int pos = getRequestType(response, responseMap);
  getHeader(response, responseMap);
  // body
  responseMap.insert({"Body", response});
  // parse header, each attribute before : will be used as key

  if (responseMap["Type"] == "Request") {
    if ((pos = responseMap["host"].find(":")) != std::string::npos) {
      std::string tmp = responseMap["host"];
      responseMap["host"] = responseMap["host"].substr(0, pos);
      responseMap.insert({"Port", tmp.substr(pos + 1, responseMap["host"].length())});
    }
    else {
      responseMap.insert({"Port", "-1"});
    }
  }
  if ((pos = responseMap["host"].find("\r")) != std::string::npos) {
    responseMap["host"] = responseMap["host"].substr(0, pos);
  }
  return responseMap;
}

void stringToLower(std::string & s) {
  for (auto iter = s.begin(); iter != s.end(); iter++) {
    *iter = std::tolower(*iter);
  }
}
