# C++多线程与线程锁

## 简介

 > 并发编程就是在多线程环境下，应用程序的执行, 
 > 能提高资源利用的效率. 尤其在IO等耗时操作(cin等半天)时有重大意义

注意 本文不讲解POSIX的pthread.h, 而是使用STL中的thread(也可以理解为boost中的thread, 两者设计方面很相似)

## 概念介绍

* 线程(thread): 线程（英语：thread）是操作系统能够进行运算调度的最小单位。大部分情况下，它被包含在进程之中，是进程中的实际运作单位。通俗来讲, 可以把一个线程理解成一名工人, 如果是单线程(一名工人), 在等待输入时会阻塞当前线程 无法继续工作, 而多线程就可以让零一名工人继续做事 这名工人来干活

* 线程锁(Thread lock): 多个线程可能同时访问/操作同一内存资源, 造成不可设想的后果(线程不安全), 为了线程安全, 就要设立线程锁 安排不同线程之间对资源的访问. C++中的线程锁有互斥锁, 条件锁, 自旋锁, 递归锁, 读写锁等

* 原子性(atomic): 要么一次做完  要不都不做. 这个比较复杂 可以理解为一次干完的事情, 比如 `cout << "value: " << value << endl;` 就不是原子性的. 非原子性的程序片段执行时可能受到其他线程影响 导致执行顺序出现问题

## 创建一个线程

本文假设您使用g++编译器和Linux. (事实上, Windows也没有问题)   
另外, 编译时您可能需要 `-pthread` 选项.   
编译指令类似这样:    

``` bash
$ g++ test.cpp -pthread -o test
```

如果您在尝试运行时出现了一些问题, 不要惊慌, 请继续往下阅读 :)

### 使用函数

``` cpp
#include <iostream>
#include <thread>
using namespace std;
void func() {cout << "I am a function!"; }
thread t(func);   //此处可以用&func
```

注意, `thread t(func);` 执行后线程会立刻开始.

### 使用有参的函数

``` cpp
void func(const char * word) {
    cout << "Hello, " << word << endl;
}
thread t(func,"World");  //"World"传参给func函数
```

### 使用函数对象

``` cpp
class functor {
public: 
    void operator()(const char *word) {
        cout << "Hello, " << word <<endl;
    }
};
functor func;
thread t(func,"World");           //直接传入对象
thread t(std::ref(func),"World"); //传入引用
```

### 注意

在成员函数中创建线程, 您可能需要

``` cpp
thread t(bind(&className::functionName,this,"args"));
```

## 线程操作

如果您尝试以上例子 可能会发现一些迷惑的现象, 如下

``` 
terminate called without an active exception
[1]    1118 abort (core dumped)  ./a.out
```

这是因为在main结束时我们创建的线程还没有结束.    
我们可以阻塞main线程. 使用   

``` cpp
thread t(func);
t.join();
```

这段代码的作用是: 等待调用t线程的线程结束(这里就是我们的主线程), 
才继续往下进行. 同时, 我们也可以使用 `detach`

``` cpp
thread t(func);
t.detach();
```

detach的线程与主线程分离 不受控制, 并且与其他线程同时执行.(而且不能再join!)     
另外 还有一个有趣的操作, 可以使当前线程停止(睡眠)一会:     

``` cpp
std::this_thread::sleep_for(std::chrono::seconds(3));
```

使当前线程睡眠三秒, 也起到阻塞线程的作用   

### 实战操作和小结:

下面我们根据所学知识实现一个计数器, 依次输出5 4 3 2 1   

``` cpp
#include <thread>
#include <iostream>
using namespace std;
int globalCounter=5;
void cnt(int n) {
    cout << "current counter number: " << n << endl;
    this_thread::sleep_for(chrono::seconds(1));
}
int main() {
    for(;globalCounter;globalCounter--) {
        thread t(cnt, globalCounter);
        t.join();
    }
    return 0;
}
```

怎么样, 是不是很简单?    
下面我们来学习**线程锁**

## 线程锁

### 互斥锁(Mutex)

顾名思义, 互斥锁保证两个线程**不会**同时访问同一对象; 
感性的理解, 一个线程执行的开始, "锁住"我这个线程要访问的资源(变量, 内存等), 
别的线程不能访问, 而我执行结束了, 就解锁这些东西.
实际上, 互斥锁的另一些无法访问执行线程锁住东西的线程占用的资源会去干别的事情 这是后话了    
我们来看代码实现, 也帮助更好理解
```cpp
#include <iostream>
#include <mutex>  //!注意这个哦
#include <thread>
#include <vector>  //STL没有boost的thread_group,但用vector<thread>也可以实现!
using namespace std;
mutex gtx;
int cnt = 0;
void ask() {
    gtx.lock();  //锁住我要的资源
    cout << ++cnt << endl;
    gtx.unlock();  //解锁
}
int main() {
    vector<thread> v;
    for (int i = 0; i < 4; i++) {
        v.emplace_back(ask);  // emplace_back与push_back的区别涉及到右值引用等问题,
                              // 可以自行查阅关于std::vector的资料
    }
    //看到这 如果你发觉要用t.join()阻塞main线程, 那么说明你学到了!
    for (thread& t : v) {  //实质是遍历v的每一个元素赋值给t, 要加上引用
        t.join();
    }
    return 0;
}
```
这段代码重点在于`gtx.lock();`和`gtx.unlock()`,请自行体会 :)    

互斥锁还有一些更高级的用法, 比如:
修改上面代码的ask函数, 
```cpp
void ask() {
    lock_guard<mutex> lg(gtx);
    cout << ++cnt << endl;
    //不需要手动解锁
}
```
这样 你甚至不需要手动解锁或加锁, 就可以实现一样的效果.   
这是什么原理呢?   
答案是RAII(具体可以自行查阅), 重点在于构造函数和析构函数. lg的作用域在void ask()的两个大括号中,
在`lock_guard<mutex>`的构造函数中它为你加锁, 离开构造函数编译器调用析构函数时在析构函数里解锁.   

同样地,我们也可以使用unique_lock来自动加锁解锁 基本与lock_guard相同,    
但unique_lock提供了更多的东西 以后还要说
```cpp
void ask() {
    unique_lock<mutex> lg(gtx);
    cout << ++cnt << endl;
    //不需要手动解锁
}
```
**另外需要注意, C++的IO流并不是线程安全的, 请单独为IO流提供线程锁!**
```cpp
void ask() {
    int i;
    {
        unique_lock<mutex> lock(gtx);
        i = ++cnt;
    }
    // 注意这两个unique_lock的作用域, 所以要加大括号
    {
        unique_lock<mutex> lock(gtx);
        cout << "count's value: " << i << endl;
    }
}
```

### 其他的锁(条件变量等)待更, 可以收藏本页面哦!

# 彩蛋
运用`this_thread::sleep_for(chrono::seconds(1));`,我们可以实现很多有趣的效果

你可能会想到`Sleep()`,但它并不是标准函数, 在windows.h中

而`sleep_for`是STL中的 跨平台.

我们可以实现一个打字机的效果 :)
```cpp
#include <thread>
#include <iostream>
using namespace std;
char word[] = "I AM EVERYWHERE.";
void print(int ind) {
    cout << word[ind];
    std::cout.flush();   // 这很重要 如果不flush stdout,那么会一起输出
    this_thread::sleep_for(chrono::seconds(1));
}
int main() {
    for(int i=0;word[i];i++) {
        thread t(print,i);
        t.join();
    }
    return 0;
}
```
我们也可以把字符串加上颜色 加一点神秘色彩 >_<
```cpp
cout << "\033[32m" << word[ind] << "\033[0m";
```

掰掰~