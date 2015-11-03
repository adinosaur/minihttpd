//
// Date: 2015/11/02
// Author: Dinosaur W.
//

#ifndef MINIHTTPD_BLOCKINGQUEUE_H
#define MINIHTTPD_BLOCKINGQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

using std::queue;
using std::mutex;
using std::condition_variable;

template <typename T>
class BlockingQueue
{
    public:
        BlockingQueue() = default;
        BlockingQueue(const BlockingQueue&) = delete;
        BlockingQueue& operator=(const BlockingQueue&) = delete;

        //
        // 队列尾部插入元素
        //
        void push_back(const T& v)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _queue.push(v);
            _cond.notify_one();
        }
        
        //
        // 返回队列首部
        //
        const T& front()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            while(_queue.empty())
                _cond.wait(lock);
            return _queue.front();
        }

        //
        // 队列首部删除元素
        //
        void pop_front()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            while (_queue.empty())
                _cond.wait(lock);
            _queue.pop();
        }

        //
        // 队列元素个数
        //
        int size()
        {
            std::unique_lock<std::mutex> lock(_mutex);
            return _queue.size();
        }
    private:
        queue<T> _queue;
        mutex _mutex;
        condition_variable _cond;
};

#endif
