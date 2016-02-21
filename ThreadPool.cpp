//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; If not, see <http://www.gnu.org/licenses/>.
//
// Copyright (C) 2016 Martin-Pierrat Louis (louismartinpierrat@gmail.com)
//

#include    <iostream>
#include    <cassert>

#include    "ThreadPool.hpp"

namespace   CSE160
{
    ThreadPool::ThreadPool(std::size_t workers_number) :
        terminate(false),
        busy_workers_number(0)
    {
        for (std::size_t i = 0 ; i < workers_number ; ++i)
        {
            this->workers.emplace_back(&ThreadPool::worker_routine, this);
        }
    }

    ThreadPool::~ThreadPool(void)
    {
        {
            std::lock_guard<std::mutex>     lock(this->mutex);
            this->terminate = true;
        }
        this->job_available.notify_all();
        for (std::thread &worker : this->workers)
            worker.join();
    }



    void
    ThreadPool::add(const std::function<void()> &new_job)
    {
        {
            std::lock_guard<std::mutex> lock(this->mutex);
            this->jobs.push(new_job);
        }
        this->job_available.notify_one();
    }

    void
    ThreadPool::wait_unfinished_jobs(void)
    {
        std::unique_lock<std::mutex>    lock(this->mutex);
        this->job_finished.wait(lock, [this]() {
            return this->jobs.empty() && this->busy_workers_number == 0;
        });
    }



    void
    ThreadPool::worker_routine(void)
    {
        while (true)
        {
            std::function<void()>   job;
            {
                std::unique_lock<std::mutex>    lock(this->mutex);
                this->job_available.wait(lock, [this](){
                    return this->terminate || !this->jobs.empty();
                });
                if (this->terminate == true) break;
                job = this->jobs.front();
                this->jobs.pop();
            }
            this->busy_workers_number += 1;
            job();
            this->busy_workers_number -= 1;
            this->job_finished.notify_all();
        }
    }

}
