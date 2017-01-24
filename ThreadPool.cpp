//
// MIT License
//
// Copyright(c) 2016 - 2017 Martin - Pierrat Louis(louismartinpierrat@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include    <iostream>
#include    <cassert>

#include    "ThreadPool.hpp"

ThreadPool::ThreadPool(std::size_t workers_number) :
    m_terminate(false),
    m_busy_workers_number(0)
{
    for (std::size_t i = 0; i < workers_number; ++i)
    {
        this->m_workers.emplace_back(&ThreadPool::worker_routine, this);
    }
}

ThreadPool::~ThreadPool(void)
{
    {
        std::lock_guard<std::mutex>     lock(this->m_mutex);
        this->m_terminate = true;
    }
    this->m_job_available.notify_all();
    for (std::thread &worker : this->m_workers)
        worker.join();
}



void
ThreadPool::add(const std::function<void()> &new_job)
{
    {
        std::lock_guard<std::mutex> lock(this->m_mutex);
        this->m_jobs.push(new_job);
    }
    this->m_job_available.notify_one();
}

void
ThreadPool::wait_unfinished_jobs(void)
{
    std::unique_lock<std::mutex>    lock(this->m_mutex);
    this->m_job_finished.wait(lock, [this]() {
        return this->m_jobs.empty() && this->m_busy_workers_number == 0;
    });
}



void
ThreadPool::worker_routine(void)
{
    while (true)
    {
        std::function<void()>   job;
        {
            std::unique_lock<std::mutex>    lock(this->m_mutex);
            this->m_job_available.wait(lock, [this]() {
                return this->m_terminate || !this->m_jobs.empty();
            });
            if (this->m_terminate == true) break;
            job = this->m_jobs.front();
            this->m_jobs.pop();
        }
        this->m_busy_workers_number += 1;
        job();
        this->m_busy_workers_number -= 1;
        this->m_job_finished.notify_all();
    }
}
