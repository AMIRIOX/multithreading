#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <cstdlib>
#include <mutex>
#include <vector>
using namespace std;

int cnt = 0;
mutex gtx;
void cnter() {
    gtx.lock();
    cout << ++cnt << endl;
    gtx.unlock();
}
int main() {
    const int SZ = 4;
    vector<thread> v;
    for(int i=0;i<SZ;i++) {
        v.emplace_back(cnter);
    }
    for(thread& t: v) {
        t.join();
    }
    return 0;
}