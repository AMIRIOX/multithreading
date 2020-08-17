#include <pthread.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
using namespace std;

int cnt = 0;
mutex gtx;
void cnter() {
    // lock_guard<mutex> lock(gtx);
    int i;
    {
        unique_lock<mutex> lock(gtx);
        i = ++cnt;
    }
    {
        unique_lock<mutex> lock(gtx);
        // gtx.lock();
        cout << "count's value: " << i << endl;
    }
    // gtx.unlock();
}
int main() {
    const int SZ = 4;
    vector<thread> v;
    for (int i = 0; i < SZ; i++) {
        v.emplace_back(cnter);
    }
    for (thread& t : v) {
        t.join();
    }
    return 0;
}