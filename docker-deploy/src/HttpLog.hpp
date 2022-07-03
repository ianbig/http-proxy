#ifndef HTTPLOG_HPP
#define HTTPLOG_HPP
#include <fstream>
#include <iostream>
#include <string>
class HttpLog {
 private:
  std::ofstream logFile;

 public:
  HttpLog() {}
  void openLogFile(std::string);
  void writeLog(std::string);
  void closeLogFile();
};
#endif
