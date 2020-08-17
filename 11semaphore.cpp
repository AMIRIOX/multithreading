#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
using namespace std;

class semaphore {
   private:
    int count;
    condition_variable cv;
    mutex mtx;

   public:
    semaphore(int cnt = 0) : count(cnt) {}
    void signal() {
        unique_lock<mutex> lock(mtx);
        count++;
        cv.notify_one();
    }
    void wait() {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [=] { return count > 0; });
        count--;
    }
} sph(1);
mutex iomtx;
string getLocalTime(const char* format) {
    auto now = std::chrono::system_clock::now();
    time_t tmt = chrono::system_clock::to_time_t(now);
    tm* time_ = localtime(&tmt);

    char str[20];
    strftime(str, sizeof str, format, time_);
    return string(str);
}
void worker() {
    sph.wait();
    thread::id tid = this_thread::get_id();
    {
        unique_lock<mutex> iol(iomtx);
        cout << "worker " << tid << " wait: " << getLocalTime("%H:%M:%S")
             << endl;
    }
    this_thread::sleep_for(chrono::seconds(1));
    sph.signal();
}
int main() {
    // cout << getLocalTime("%H:%M:%S") << endl;
    vector<thread> v;
    v.reserve(3);
    for (int i = 0; i < 3; i++) {
        v.emplace_back(&worker);
    }
    for (thread& t : v) {
        t.join();
    }
    return 0;
}