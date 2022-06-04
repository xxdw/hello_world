#include <deque>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <atomic>
#include <future>
#include <functional>
#include <iostream>

//
// Created by 79203 on 2022/6/3.
//
template <typename T>
class SafeQueue {
public:
    SafeQueue():max_queue_size_(100) {}
    SafeQueue(int max_queue_size):max_queue_size_(max_queue_size) {}

    SafeQueue(const SafeQueue<T>& safe_queue):max_queue_size_(safe_queue.max_queue_size_) {}

    void push(T&& t) {
        std::unique_lock<std::mutex> lock(mutex_);
        while (deque_.size() >= max_queue_size_) { cv_full_.wait(lock); }
        deque_.template emplace_back(std::forward<T>(t));
        cv_empty_.notify_one();
    }

    T&& pop() {
       std::unique_lock<std::mutex> lock(mutex_);
       while (deque_.size()<=0) { cv_empty_.wait(lock); }
       T&& t = std::move(deque_.front());
       deque_.pop_front();
       cv_full_.notify_one();
       return std::move(t);
    }

    void NotifyAll() {
       cv_full_.notify_all();
       cv_empty_.notify_all();
    }

public:
    std::deque<T> deque_;
    int max_queue_size_;
    std::mutex mutex_;
    std::condition_variable cv_full_;
    std::condition_variable cv_empty_;
};

class ThreadPool {
public:
    ThreadPool(int thread_size, int queue_size = 100):
         thread_size_(thread_size), safe_queue_(queue_size),stop_(false){ }
         
    void Start() {
        for (int i=0; i<thread_size_; ++i) {
            m_thread_.emplace_back(&ThreadPool::Loop, this);
        }
    }

    template<typename Func, typename... Args>
    auto Schedule(Func&& f, Args &&... args) -> std::future<typename std::result_of<Func(Args...)>::type> {
        using return_type = typename std::result_of<Func(Args...)>::type;
        auto task = std::make_shared< std::packaged_task<return_type()> >(
                std::bind(std::forward<Func>(f), std::forward<Args>(args)...)
        );
        std::future<return_type> res = task->get_future();
        safe_queue_.push([task](){ (*task)(); });
        return res;
    }
    void Stop() {
      stop_ = false;
      safe_queue_.NotifyAll();
      for (auto& th:m_thread_) {
          th.join();
      }
    }

    ThreadPool(const ThreadPool& th) = delete;
    ThreadPool& operator=(const ThreadPool& th) = delete;
    ~ThreadPool() {
       if (stop_) {
           Stop();
       }
    }
protected:
    void Loop() {
        while (!stop_) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(safe_queue_.mutex_);
                while (safe_queue_.deque_.size()<=0) { safe_queue_.cv_empty_.wait(lock); }
                task = std::move(safe_queue_.deque_.front());
                safe_queue_.deque_.pop_front();
            }
            task();
        }
    }
private:
    int thread_size_;
    SafeQueue<std::function<void()>> safe_queue_;
    std::vector<std::thread> m_thread_;
    std::atomic<bool> stop_;
};

int  main()  {
    ThreadPool thread_pool(10,100);
    thread_pool.Start();
    auto res = thread_pool.Schedule([](int i)->int{
        while (i>0) {
            std::cout << i << std::endl;
            --i;
        }
        return i;
    }, 100);
    std::cout << res.get() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}