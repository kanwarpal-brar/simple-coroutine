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
        std::cout << "Anticipating Error------" << std::endl;
        root();  // Should error:
    } catch (Coroutine<Node>::FinishedException& e) {
        std::cout << "Caught FinishedException" << std::endl;
    }
}

void creation() {
    Coroutine<SimpleCoroutine> cor = Coroutine<SimpleCoroutine>();
    cor();
    cor();
}

class LoopingCoroutine : public BaseCoroutine {
    int j = 0;
    void main() override {
        for (int i = 0; i < INT_MAX; i++) {
            j++;
            std::cout << j << std::endl;
            suspend();
        }
    }
};

int main() {
    Coroutine<LoopingCoroutine> cor = Coroutine<LoopingCoroutine>();  // Create
    while (!cor.is_finished()) {
        cor();  // Swap context
    }
}