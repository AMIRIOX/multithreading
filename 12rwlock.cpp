#include <condition_variable>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>
using namespace std;
class cnter {
   public:
    cnter() : count(0) {}
    void inrc() {
        unique_lock<std::shared_mutex> lock(mtx);
        count++;
    }
    size_t ask() {
        shared_lock<std::shared_mutex> lock(mtx);
        return count;
    }
    void reset() {
        unique_lock<std::shared_mutex> lock(mtx);
        count = 0;
    }

   private:
    size_t count;
    mutable shared_mutex mtx;
};
mutex iolock;
void worker(cnter& c) {
    for (int i = 0; i < 3; i++) {
        c.inrc();
        size_t val = c.ask();
        {
            lock_guard<mutex> lock(iolock);
            cout << "thread " << this_thread::get_id() << ' ' << val << endl;
        }
    }
}
int main() {
    cnter cntp;
    vector<thread> v;
    v.reserve(3);
    for (int i = 0; i < 3; i++) {
        v.emplace_back(&worker, ref(cntp));
    }
    for (thread& t : v) {
        t.join();
    }
    return 0;
}