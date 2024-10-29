#include <iostream>
#include "corjob.h"

int main() {
    CorJob job;
    std::cout << "back to main" << std::endl;
    std::cout << "isdone1: " << job.is_done() << std::endl;
    job();
    std::cout << "isdone2: " << job.is_done() << std::endl;
}