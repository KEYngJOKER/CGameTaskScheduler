#pragma once

#include <cstddef>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <future>
#include <functional>
#include <vector>
#include <condition_variable>

class TaskScheduler
{
public:
    enum class ETaskPriority : int{
        HIGH = 2,
        NORMAL = 1,
        LOW = 0
    };

    explicit TaskScheduler(size_t threadCount = std::thread::hardware_concurrency())
        : _stop(false), _seq(0)
    {
        if(threadCount == 0) threadCount = 1;
        _workers.reserve(threadCount);
        for(size_t i = 0; i < threadCount; ++i){
            _workers.emplace_back([this] {workerLoop();});
        }
    }
    
    ~TaskScheduler(){
        shutdown();
    }
    template <typename F, typename... Args>
    auto submit(ETaskPriority pri, F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using Ret = typename std::result_of<F(Args...)>::type;
        
        auto bound = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        auto task  = std::make_shared<std::packaged_task<Ret()>>(std::move(bound));
        std::future<Ret> fut = task->get_future();
        {
            std::lock_guard<std::mutex> lk(_mtx);
            if (_stop) throw std::runtime_error("submit on stopped TaskScheduler");
            _queue.push(TaskItem{
                static_cast<int>(pri),                      
                _seq.fetch_add(1, std::memory_order_relaxed), 
                [task]{ (*task)(); }                        
            });
    }
    _cv.notify_one();
    return fut;
    }
    TaskScheduler(const TaskScheduler&) = delete;
    TaskScheduler& operator=(const TaskScheduler&) = delete;

    //等待队列清空再退出；如需立即终端 使用下面shutdownNow()
    void shutdown(){
        bool expected = false;
        if(_stop.compare_exchange_strong(expected, true)){
            _cv.notify_all();
            for(auto& t : _workers) if (t.joinable()) t.join();
        }
    }
    //强制退出 清空队列并使线程结束 (未执行的任务会被丢弃)
    void shutdownNow(){
        bool expected = false;
        if(_stop.compare_exchange_strong(expected, true)){
            std::lock_guard<std::mutex> lock (_mtx);
            while(!_queue.empty()) _queue.pop();
        }
        _cv.notify_all();
        for(auto& t : _workers) if (t.joinable()) t.join();
    }
    size_t pending() const{
        std::lock_guard<std::mutex> lock(_mtx);
        return _queue.size();
    }

private:
    struct TaskItem
    {
        int iTaskPriority;      //任务优先级 越大越高
        uint64_t seq;           //同优先级FIFO
        std::function<void()> function;
    };

    struct Cmp
    {
        bool operator()(const TaskItem& a, const TaskItem& b) const{
            if (a.iTaskPriority != b.iTaskPriority) return a.iTaskPriority < b.iTaskPriority;
            return a.seq > b.seq;     //同优先级按顺序提交
        }
    };

    void workerLoop(){
        for(;;){
            TaskItem item;
            {
                std::unique_lock<std::mutex> u_lk(_mtx);
                _cv.wait(u_lk, [this] {return _stop || !_queue.empty();});
                if (_stop && _queue.empty()) return;
                item = _queue.top();
                _queue.pop();
            }
            try { item.function(); }
            catch(...){}
        }
    }
    std::priority_queue<TaskItem, std::vector<TaskItem>, Cmp> _queue;
    std::condition_variable _cv;
    mutable std::mutex _mtx;
    std::vector<std::thread> _workers;
    std::atomic<bool> _stop;
    std::atomic<uint64_t> _seq;
};