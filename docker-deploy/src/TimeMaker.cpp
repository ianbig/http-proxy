#include "TimeMaker.hpp"

#include <time.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
double TimeMaker::getDateNowDiff(std::string time_str) {
  if (time_str.find(" GMT") != std::string::npos) {
    time_str = time_str.substr(0, time_str.length() - 4);
  }
  time_t now;
  time(&now);
  double seconds;
  struct tm * now_struct = localtime(&now);
  struct tm time_struct;
  strptime(time_str.c_str(), "%a, %d %b %Y %H:%M:%S", &time_struct);
  seconds = difftime(mktime(&time_struct), mktime(&(*now_struct)));  // end - begin
  return seconds;
}
