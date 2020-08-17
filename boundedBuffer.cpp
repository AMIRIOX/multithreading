#include <pthread.h>
#include <unistd.h>

#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;

class bBuffer {
   private:
    vector<int> cacBuffer;
    size_t begin, end, buffered;
    condition_variable notFullCV;
    condition_variable notEmptyCV;
    mutex mutexLock;

   public:
    bBuffer(const bBuffer& t)=delete;
    bBuffer& operator=(const bBuffer& t)=delete;
    bBuffer(size_t sz): begin(0),end(0),buffered(0),cacBuffer(sz) {}
    void produce(int n) {
        {
            unique_lock<mutex> lock(mutexLock);
            notFullCV.wait(lock, [=] { return buffered < cacBuffer.size(); });
            cacBuffer[end] = n;
            end = (end + 1) % cacBuffer.size();
            ++buffered;
        }
        notEmptyCV.notify_one();
    }
    int consume() {
        unique_lock<mutex> lock(mutexLock);
        notEmptyCV.wait(lock, [=] { return buffered > 0; });
        int n = cacBuffer[begin];
        begin = (begin + 1) % cacBuffer.size();
        --buffered;
        lock.unlock();
        notFullCV.notify_one();
        return n;
    }
} buff(2);
mutex ioLock;
void producer() {
    int n = 0;
    while (n < 100000) {
        buff.produce(n);
        if (!(n % 10000)) {
            unique_lock<mutex> iol(ioLock);
            cout << "Produce: " << n << endl;
        }
        ++n;
    }
    buff.produce(-1);
}
void consumer() {
    thread::id threadID = this_thread::get_id();
    int n = 0;
    do {
        n = buff.consume();
        if (!(n % 10000)) {
            unique_lock<mutex> iol(ioLock);
            cout << "Consume: " << n << '(' << threadID << ')' << endl;
        }
    } while (n != -1);
    buff.produce(-1);
}
int main() {
    vector<thread> v;

    v.push_back(thread(producer));
    v.push_back(thread(consumer));
    v.push_back(thread(consumer));
    v.push_back(thread(consumer));

    for (thread& t : v) {
        t.join();
    }

    return 0;
}