#include "CacheController.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "LRUCache.hpp"
#include "TimeMaker.hpp"

CacheController::CacheController(size_t size) {
  cache = new LRUCache(size);
}

CacheController::~CacheController() {
  delete cache;
}

void CacheController::putInCache(std::string url_str,
                                 std::map<std::string, std::string> resMap) {
  cache->put(url_str, resMap);
}

std::string CacheController::getCache(std::string url) {
  std::map<std::string, std::string> headerMap = cache->getResponse(url);
  return headerMap["Body"];
}

bool CacheController::toRevalidate(std::string url_string,
                                   HttpLog & logger,
                                   std::string id) {
  std::map<std::string, std::string> responseMap = cache->getResponse(url_string);
  if (responseMap.empty()) {
    // no response in cache
    std::string logMsg = id + ": not in cache";
    logger.writeLog(logMsg);
    return true;
  }
  std::string cacheControl_str("");
  std::string pragma_str("");
  std::string vary_str("");
  std::string expires_str("");
  if (responseMap.find("cache-control") != responseMap.end()) {
    cacheControl_str = responseMap.find("cache-control")->second;
    if (cacheControl_str.find("max-age=") != std::string::npos) {
      double maxAge = getMaxage(cacheControl_str);
      TimeMaker myTM;
      double diff = maxAge + myTM.getDateNowDiff(responseMap.find("date")->second);
      if (diff <= 0) {
        std::string msg = id + ": cached, expires at " + responseMap["date"];
        logger.writeLog(msg);
        return true;
      }

      else {
        std::string msg = id + ": in cache, valid";
        logger.writeLog(msg);
        return false;
      }
    }
  }
  if (responseMap.find("pragma") != responseMap.end()) {
    pragma_str = responseMap.find("pragma")->second;
  }
  if (responseMap.find("vary") != responseMap.end()) {
    vary_str = responseMap.find("vary")->second;
  }
  if (responseMap.find("etag") != responseMap.end()) {
    std::string msg = id + ": in cache, requires validation";
    logger.writeLog(msg);
    return true;
  }
  if (responseMap.find("expires") != responseMap.end()) {
    expires_str = responseMap.find("expires")->second;
  }
  if (!checkCacheControl(cacheControl_str) && !checkPragma(pragma_str) &&
      !checkVary(vary_str) && !checkExpires(expires_str)) {
    std::string msg = id + ": in cache, valid";
    logger.writeLog(msg);
    return false;
  }

  if (checkCacheControl(cacheControl_str) || checkPragma(pragma_str) ||
      checkVary(vary_str) || checkExpires(expires_str)) {
    return true;
  }

  std::string msg = id + ": in cache, valid";
  logger.writeLog(msg);
  return false;
}

bool CacheController::checkExpires(std::string expires_str) {
  if (expires_str.length() <= 1) {
    return false;
  }
  if (expires_str.find(" GMT") != std::string::npos) {
    expires_str = expires_str.substr(0, expires_str.length() - 4);
  }
  time_t now;
  time(&now);
  double seconds;
  struct tm * now_struct = localtime(&now);
  struct tm expire_struct;
  struct tm timeRef_struct;
  std::string ref_time = "Fri, 12 Feb 2022 15:20:01";
  strptime(expires_str.c_str(), "%a, %d %b %Y %H:%M:%S", &expire_struct);
  strptime(ref_time.c_str(), "%a, %d %b %Y %H:%M:%S", &timeRef_struct);
  double expireElapse = difftime(mktime(&expire_struct), mktime(&timeRef_struct));
  double nowElapse = difftime(mktime(&(*now_struct)), mktime(&timeRef_struct));
  return expireElapse < nowElapse;
}

bool CacheController::checkCacheControl(std::string cacheControl_str) {
  std::string targetHeader[]{"no-store", "no-cache", "must-revalidate"};
  std::vector<std::string> header_vec(
      targetHeader, targetHeader + sizeof(targetHeader) / sizeof(std::string));
  if (cacheControl_str.length() == 0) {
    return true;
  }
  int pos;
  for (auto it = header_vec.begin(); it != header_vec.end(); it++) {
    pos = cacheControl_str.find(*it);
    if (pos != std::string::npos) {
      return true;
    }
  }

  return false;
}
int CacheController::getMaxage(std::string cacheControl_str) {
  int pos;
  int age = 0;
  if ((pos = cacheControl_str.find("max-age=")) != std::string::npos) {
    age = std::stoi(cacheControl_str.substr(pos + 8, cacheControl_str.length()));
  }

  return age;
}

bool CacheController::checkPragma(std::string pragma_str) {
  std::string targetHeader[]{"no-cache"};
  std::vector<std::string> header_vec(
      targetHeader, targetHeader + sizeof(targetHeader) / sizeof(std::string));
  if (pragma_str.length() == 0) {
    return true;
  }
  for (auto it = header_vec.begin(); it != header_vec.end(); it++) {
    int pos = pragma_str.find(*it);
    if (pos != std::string::npos) {
      return true;
    }
  }
  return false;
}
bool CacheController::checkVary(std::string vary_str) {
  if (vary_str.length() == 0) {
    return true;
  }
  return false;
}
