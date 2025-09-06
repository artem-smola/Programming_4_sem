#include <iostream>
#include "logger.hpp"

int main(){
    ConcurrentLogger logger(100, 10, 1000);
    logger.ExecuteCase1();
    logger.ExecuteCase2();
    logger.ExecuteCase3();
    logger.ExecuteCase4();
    return 0;
}