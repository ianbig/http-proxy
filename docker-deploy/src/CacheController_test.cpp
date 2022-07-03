#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include "CacheController.hpp"
#include "TimeMaker.hpp"
#include <fstream>
#include <string>
int main()
{
    unsigned size = 4;
    LRUCache cache(4);
    std::string test[8]{"try1", "try2", "try1", "try4", "try5", "try6", "try7", "try8"};
    std::map<std::string, std::string> response_Map;
    for (unsigned i = 0; i < size; i++)
    {
        response_Map.insert(std::pair<std::string, std::string>(std::to_string(i + 1), test[i]));
        cache.put(test[i], response_Map);
    }
    CacheController myController(cache);
    std::string line("");
    std::string lines("");
    std::ifstream myFile("yes.txt");
    if (myFile.is_open())
    {
        while (std::getline(myFile, line))
        {
            lines += line + "\n";
        }
        myFile.close();
        if(myController.checkVary(lines)){
            std::cout << "need to revalidate" << std::endl;
        } else {
            std::cout << "no need" << std::endl;
        }
        //std::cout << myController.getMaxage("max-age=7200") << std::endl;
    }
    TimeMaker tm;
    //std::cout << tm.getTimeInSec("Fri, 18 Feb 2022 15:11:01")<< std::endl;
    std::cout << myController.checkExpires("Fri, 18 Feb 2022 15:11:01") << std::endl;
    return 0;
}
