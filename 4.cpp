#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <cstdlib>
int cnt = 5;

using std::cout; using std::endl;
void func(int n) {
    cout << n << endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
}
int main() {
    //std::thread t(func);
    for(;cnt;cnt--) {
        std::thread t(func,cnt);
        //t.detach();
        t.join();
    }
    return 0;
}