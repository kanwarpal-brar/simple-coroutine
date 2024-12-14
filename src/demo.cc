#include <iostream>
#include "corjob.h"
#include "node.h"

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
        root();
        root();
        root();
        root();
    } catch (Node::FinishedException& e) {
        std::cout << "Caught FinishedException" << std::endl;
    }
}

int main() {
    node();
}