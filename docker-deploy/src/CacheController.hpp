#ifndef CACHECONTROLLER_HPP
#define CACHECONTROLLER_HPP
#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "HttpLog.hpp"
#include "LRUCache.hpp"

class CacheController {
  LRUCache * cache;
  bool checkCacheControl(std::string);
  bool checkPragma(std::string);
  bool checkVary(std::string);
  int getMaxage(std::string);
  bool checkExpires(std::string);
  std::string controlParser();

 public:
  CacheController(size_t size);
  CacheController() {}
  ~CacheController();
  void putInCache(std::string, std::map<std::string, std::string>);
  bool toRevalidate(std::string, HttpLog &, std::string);
  std::string getCache(std::string url);
};

#endif
