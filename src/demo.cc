#include <iostream>
#include "corjob.h"

int main() {
    try {
        CorJob job;
        job();
        job();
        job();
        job();
    } catch (CorJob::FinishedException& e) {
        std::cout << "Caught CorJobFinishedException" << std::endl;
    }
}