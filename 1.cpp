#include <iostream>
#include <thread>
using namespace std;
int cnt = 0;
void func() {
    cout << "cur thread is starting running with code ";
    cout << ++cnt << "! \n";
    this_thread::sleep_for(chrono::seconds(3));
    cout << "exiting cur thread...\n";
}
void caller() {
    cout << "this thread, it's a caller.\n";
    thread t(func);
    // t.join();
    t.detach();
    this_thread::sleep_for(chrono::seconds(1));
    cout << "exiting this main thread, it's a caller.\n";
}
int main() {
    caller();
    this_thread::sleep_for(chrono::seconds(5));
    return 0;
}