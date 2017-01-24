# Introduction

[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE)

This is a simple generic threadpool in c++.

# Example

```c++
#include    "ThreadPool.hpp"

#include    <iostream>

std::ostream &operator<<(std::ostream &os, const std::vector<std::thread::id> &thread_ids)
{
    for (auto thread_id : thread_ids)
    {
        os << "[" << thread_id << "]";
    }
    os << std::endl;
    return os;
}

int     main(void)
{
    ThreadPool  threadpool(std::thread::hardware_concurrency());
    std::vector<std::thread::id>    thread_ids(1000, std::this_thread::get_id());

    for (std::size_t job_number = 0; job_number < 1000; ++job_number)
    {
        threadpool.add([=, &thread_ids]() {
            thread_ids[job_number] = std::this_thread::get_id();
        });
    }
    threadpool.wait_unfinished_jobs();
    std::cout << thread_ids << std::endl;
    std::cin.ignore();
}
```