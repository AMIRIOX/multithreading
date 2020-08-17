#include <iostream>
#include <thread>
using namespace std;
char word[] = "I AM EVERYWHERE.";
void print(int ind) {
    // cout << word[ind];
    cout << "\033[31m" << word[ind] << "\033[0m";
    std::cout.flush();
    this_thread::sleep_for(chrono::seconds(1));
}
int main() {
    for (int i = 0; word[i]; i++) {
        thread t(print, i);
        t.join();
    }
    return 0;
}