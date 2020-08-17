#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <cstdlib>
using namespace std;
void hello(const char * word) {
    cout << "I am still wating but...";
    this_thread::sleep_for(chrono::seconds(5));
    cout << "no problem. He tell me: \n";
    cout << word << "." << endl;
}
int main() {   
    thread t(hello,"I love you forever");
    t.join();
    //t.detach();
    //cout << "xxx";
    return 0;
}