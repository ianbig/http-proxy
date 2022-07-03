#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <algorithm>

#include "LRUCache.hpp"
int main()
{
    unsigned size = 8;
    std::string test[8]{"try1", "try2", "try1", "try4", "try5", "try6", "try7", "try8"};
    LRUCache ca(size / 2);
    ca.getCapacity();
    std::map<std::string, std::string> response_Map;
    for (unsigned i = 0; i < size; i++)
    {
        response_Map.insert(std::pair<std::string, std::string>(std::to_string(i + 1), test[i]));
        ca.put(test[i], response_Map);
        //ca.display();
    }
    //std::thread first(&LRUCache::put, &ca, "try5", response_Map);
    // std::thread second(&LRUCache::put, &ca, "try5", response_Map);
    //std::thread third(&LRUCache::put, &ca, "try5", response_Map);
    std::map<std::string, std::string> newMap = ca.getResponse("try5");
    if (newMap.empty())
    {
        std::cout << "no such url" << std::endl;
    }
    else
    {
        std::cout << newMap.size() << std::endl;
    }
    // first.join();
    //second.join();
    //third.join();
    return 0;
}
