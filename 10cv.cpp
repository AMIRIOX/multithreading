#include <pthread.h>
#include <unistd.h>

#include <condition_variable>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>
using namespace std;

mutex gtx;
condition_variable cv;
string data;
bool processed = false, ready = false;
void work() {
    unique_lock<mutex> lock(gtx);
    cv.wait(lock, [] { return ready; });

    cout << "work is processing data..." << endl;
    this_thread::sleep_for(chrono::seconds(2));
    data += " and processor done.";

    processed = true;
    cout << "processor ok." << endl;

    lock.unlock();
    cv.notify_one();
}

int main() {
    thread worker(work);
    {
        unique_lock<mutex> melock(gtx);
        cout << "program starting..." << endl;
        this_thread::sleep_for(chrono::seconds(1));
        data = "test data";
        ready = true;
        cout << "data is ok." << endl;
    }
    cv.notify_one();
    {
        unique_lock<mutex> mlock(gtx);
        cv.wait(mlock, [] { return processed; });
    }
    cout << "accept data: " << data << endl;
    worker.join();
    return 0;
}