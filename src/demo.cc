#include <iostream>
#include "corjob.h"
#include "node.h"
#include <chrono>
#include "simple_coroutine.h"

void basic() {
    try {
        CorJob job;
        job();
        job();
        job();
        job();
    } catch (CorJob::FinishedException& e) {
        std::cout << "Caught FinishedException" << std::endl;
    }
}

void node() {
    try {
        Coroutine<Node> root = Coroutine<Node>(3);
        while (!root.is_finished()) {
            root();
        }
    } catch (Coroutine<Node>::FinishedException& e) {
        std::cout << "Caught FinishedException" << std::endl;
    }
}

void creation() {
    Coroutine<SimpleCoroutine> cor = Coroutine<SimpleCoroutine>();
    cor();
}

int main() {
    creation();
}