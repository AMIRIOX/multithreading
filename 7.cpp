#include <pthread.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <thread>
using namespace std;
class cnt {
   private:
    int val;

   public:
    cnt(int n) : val(n) {}
    void operator()() {
        while (val) {
            cout << val-- << " ";
            std::this_thread::sleep_for(chrono::seconds(1));
        }
        cout << endl;
    }
};
int main() {
    thread t(cnt(3));
    t.join();

    thread t1(cnt(5));
    t1.detach();

    this_thread::sleep_for(chrono::seconds(5));
    return 0;
}

// #include <iostream>
// #include <thread>
// class Counter {
//    public:
//     Counter(int value) : value_(value) {}
//     void operator()() {
//         while (value_ > 0) {
//             std::cout << value_ << " ";
//             --value_;
//             std::this_thread::sleep_for(std::chrono::seconds(1));
//         }
//         std::cout << std::endl;
//     }

//    private:
//     int value_;
// };
// int main() {
//     std::thread t1(Counter(3));
//     t1.join();
//     std::thread t2(Counter(3));
//     t2.detach();  // 等待几秒，不然 t2 根本没机会执行。
//     std::this_thread::sleep_for(std::chrono::seconds(4));
//     return 0;
// }
