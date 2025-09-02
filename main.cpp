#include "TaskScheduler.h"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    // 创建调度器，4 个线程
    TaskScheduler scheduler(4);

    // 提交几个任务
    auto f1 = scheduler.submit(TaskScheduler::ETaskPriority::LOW, [] {
        std::cout << "[LOW] Task 1 running on thread " 
                  << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return 1;
    });

    auto f2 = scheduler.submit(TaskScheduler::ETaskPriority::HIGH, [] {
        std::cout << "[HIGH] Task 2 running on thread " 
                  << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        return 2;
    });

    auto f3 = scheduler.submit(TaskScheduler::ETaskPriority::NORMAL, [] {
        std::cout << "[NORMAL] Task 3 running on thread " 
                  << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        return 3;
    });

    auto f4 = scheduler.submit(TaskScheduler::ETaskPriority::HIGH, [] {
        std::cout << "[HIGH] Task 4 running on thread " 
                  << std::this_thread::get_id() << std::endl;
        return 4;
    });

    // 等待任务完成并获取结果
    std::cout << "Result f1: " << f1.get() << std::endl;
    std::cout << "Result f2: " << f2.get() << std::endl;
    std::cout << "Result f3: " << f3.get() << std::endl;
    std::cout << "Result f4: " << f4.get() << std::endl;

    scheduler.shutdown(); // 等待任务执行完再退出
    return 0;
}
