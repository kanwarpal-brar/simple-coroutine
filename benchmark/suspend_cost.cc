#include "../src/corjob.h"

class SuspendingCoroutine : public BaseCoroutine {
    void main() override {
        int i = 0;
        suspend();
        i++;
        return;
    }
};

int main() {
    Coroutine<SuspendingCoroutine> cor = Coroutine<SuspendingCoroutine>();  // Create
    cor();
    cor();
}