
#include "../include/Base/BlockingQueue.h"
#include <iostream>
#include <string>
#include <thread>

using namespace std;

BlockingQueue<string> q;

void func()
{
    for (int i = 0; i != 100; ++i)
        q.push_back(std::to_string(i));
}

int main()
{
    thread thread1(func);
    thread thread2(func);

    thread1.join();
    thread2.join();

    int size = q.size();
    for (int i = 0; i != size; ++i)
    {
        cout << q.front() << endl;
        q.pop_front();
    }
    return 0;
}
