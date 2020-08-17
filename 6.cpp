#include <pthread.h>
#include <unistd.h>

#include <cstdlib>
#include <functional>
#include <iostream>
#include <thread>
using namespace std;
class demo {
   public:
    demo() {
        std::thread t(bind(&demo::cr, this, "world"));
        t.join();
    }

   private:
    void cr(const char* word) {
        std::this_thread::sleep_for(chrono::seconds(2));
        cout << word << endl;
    }
};
int main() { demo d; }