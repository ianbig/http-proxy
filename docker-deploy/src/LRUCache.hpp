#ifndef LRUCACHE_HPP
#define LRUCACHE_HPP

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class LRUCache {
  unsigned int capacity;
  std::vector<std::string> url_vector{};
  std::vector<std::map<std::string, std::string> > res_Map_vector{};

 public:
  LRUCache(unsigned);
  void put(std::string, std::map<std::string, std::string>);
  std::map<std::string, std::string> getResponse(std::string);
  void display();
  void getCapacity();
  LRUCache() : capacity(0) {}
  ~LRUCache() {}
};

#endif
