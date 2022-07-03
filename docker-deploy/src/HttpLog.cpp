#include "HttpLog.hpp"

#include <stdlib.h>

#include <fstream>
#include <iostream>
#include <string>
pthread_mutex_t lock;

void HttpLog::openLogFile(std::string path) {
  if (!logFile.is_open()) {
    logFile.open(path, std::ofstream::app);
  }
}

void HttpLog::closeLogFile() {
  logFile.close();
}

void HttpLog::writeLog(std::string toWrite) {
  if (logFile.is_open()) {
    pthread_mutex_lock(&lock);
    logFile << toWrite << std::endl;
    pthread_mutex_unlock(&lock);
  }
}
