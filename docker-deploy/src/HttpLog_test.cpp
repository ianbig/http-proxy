#include <iostream>
#include <string>
#include <fstream>
#include "HttpLog.hpp"
#include <thread>

void testWriteLog(HttpLog & myLog){
    myLog.writeLog("hello");
    myLog.writeLog("mother");
}

int main(){
    HttpLog myLog;
    myLog.openLogFile();
    std::thread first(testWriteLog, std::ref(myLog));
    std::thread second(testWriteLog, std::ref(myLog));
    std::thread third(testWriteLog, std::ref(myLog));
    first.join();
    second.join();
    third.join();
    myLog.closeLogFile();
    return 0;
}

