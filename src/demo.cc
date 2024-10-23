#include <iostream>
#include "corjob.h"

int main() {
    CorJob job;
    job.a = 10;
    std::cout << "Hello " << job.a << std::endl;
    return 0;
}