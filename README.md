# TaskSchedulerDemo

一个基于 **C++17** 的简易任务调度系统 Demo，支持：

- 多线程任务执行
- 优先级调度（High / Normal / Low）
- 使用 `std::future` 获取任务返回值
- 自动化编译、运行和清理脚本

---

## 🛠️ 环境要求

- CMake >= 3.10
- GCC / Clang (支持 C++17)
- Linux / macOS  
  （Windows 需要 MinGW 或 Visual Studio，暂未写专门脚本）

---

## 📂 项目结构
TaskSchedulerDemo/
├── CMakeLists.txt # CMake 配置文件
├── TaskScheduler.h # 任务调度器实现
├── main.cpp # 示例代码入口
├── build.sh # 自动编译 + 运行脚本
└── clean.sh # 清理脚本


---

## 🚀 使用方法

### 1. 编译 & 运行
```bash
chmod +x build.sh
./build.sh

编译成功后会自动运行 ./test，你会看到类似输出：

High priority task running
Normal priority task running
Low priority task running
Normal task result: 42
```
### 2. 清理
```
chmod +x clean.sh
./clean.sh
```
会直接删除 build/ 目录。

📌 示例说明
```
main.cpp 中演示了三种任务提交：

auto f1 = scheduler.submit(TaskScheduler::ETaskPriority::HIGH, [] {
    std::cout << "High priority task running\n";
});

auto f2 = scheduler.submit(TaskScheduler::ETaskPriority::LOW, [] {
    std::cout << "Low priority task running\n";
});

auto f3 = scheduler.submit([] {
    std::cout << "Normal priority task running\n";
    return 42;
});

```
高优先级任务会优先执行

普通任务演示了有返回值的情况

通过 future.get() 获取结果

📖 TODO

 增加定时任务功能

 增加任务取消功能