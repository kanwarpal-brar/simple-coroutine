#include "corjob.h"

class Node : public BaseCoroutine {
    int value;
    Node* left = nullptr;
    Node* right = nullptr;

    void main() override {
        PRINT("Node Initied");
        PRINT(value);
        PRINT("N1");
        suspend();
        PRINT("N2");
        suspend();
        PRINT("N3");
    }

  public:
    Node(int value) : value(value) {}
};