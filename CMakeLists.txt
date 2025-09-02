cmake_minimum_required(VERSION 3.10)
project(TaskSchedulerDemo CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(test
    main.cpp
    TaskScheduler.h
)

target_link_libraries(test PRIVATE Threads::Threads)
