#include "../src/corjob.h"

const int max = INT_MAX / 2;

class LoopingCoroutine : public BaseCoroutine {
    int j = 0;
    void main() override {
        for (int i = 0; i < max; i++) {
            j++;
            suspend();
        }
    }
};

int main() {
    // Coroutine<LoopingCoroutine> cor = Coroutine<LoopingCoroutine>();  // Create
    // while (!cor.is_finished()) {
    //     cor();  // Swap context
    // }
    int j = 0;
    for (int i = 0; i < max; i++) {
            j++;

    }
}