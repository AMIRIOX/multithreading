#include <pthread.h>
#include <unistd.h>

#include <cstdlib>
#include <iostream>
#include <thread>

const int threadNum = 5;
using std::cout;
using std::endl;
void* func(void* args) {
    // std::cout << "hello" << std::endl;
    // std::cout << std::this_thread::get_id() << std::endl;;
    int tid = *((int*)args);
    cout << "this thread id: " << tid << endl;
    pthread_exit(nullptr);
    return 0;
}
int main(int argc, char const* argv[]) {
    pthread_t tid[threadNum];
    int inds[threadNum];
    for (int i = 0; i < threadNum; i++) {
        inds[i] = i;
        int returnValue =
            pthread_create(&tid[i], nullptr, func, (void*)&inds[i]);
        if (returnValue != 0)
            std::cout << "thread  error code: " << returnValue << std::endl;
    }
    // pthread_create
    pthread_exit(nullptr);
    return 0;
}
