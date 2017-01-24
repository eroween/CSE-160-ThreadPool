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

#pragma once

#include    <vector>
#include    <queue>
#include    <functional>
#include    <thread>
#include    <mutex>
#include    <atomic>
#include    <condition_variable>

///
/// \brief This class is an implementation of the design pattern thread pool.
///
/// More informations can be found here : https://en.wikipedia.org/wiki/Thread_pool
///
class   ThreadPool
{
public:
    ///
    /// \brief Create a default thread pool with "workers_number" workers.
    ///
    ThreadPool(std::size_t workers_number);

    ///
    /// \brief Copy constructor.
    ///
    /// We must delete the copy constructor because of the non copyable
    /// type of a std::condition_variable.
    ///
    ThreadPool(const ThreadPool &other) = delete;

    ///
    /// \brief Move constructor.
    ///
    /// We must delete the move constructor because of the non movable type
    /// of a std::condition_variable.
    ///
    ThreadPool(ThreadPool &&other) noexcept = delete;

    ///
    /// \brief Destructor.
    ///
    virtual ~ThreadPool(void) noexcept;

public:
    ///
    /// \brief Copy assignment operator.
    ///
    /// We must delete the copy assignment operator because of the
    /// non copyable type of a std::condition_variable.
    ///
    ThreadPool  &operator=(const ThreadPool &other) = delete;

    ///
    /// \brief Move assignment operator.
    ///
    /// We must delete the move constructor because of the non movable type
    /// of a std::condition_variable.
    ///
    ThreadPool  &operator=(ThreadPool &&other) noexcept = delete;

public:
    ///
    /// \brief Add a new job in the jobs queue.
    ///
    /// We use a std::function to implement a job.
    ///
    void    add(const std::function<void()> &new_job);

    ///
    /// \brief Wait until all jobs are finished.
    ///
    /// This method wait until the queue is empty and the  busy_workers_number
    /// is equal to 0.
    ///
    void    wait_unfinished_jobs(void);

private:
    ///
    /// \brief This method is the worker routine. All worker are launched with this routine.
    ///
    void    worker_routine(void);

private:
    ///
    /// \brief Used to stop worker, they are waiting for a new task
    /// or when terminate is set to true.
    ///
    /// Terminate is considered as a job.
    ///
    bool    m_terminate;

    ///
    /// \brief Number of workers actually processing a job.
    ///
    std::atomic<std::size_t>    m_busy_workers_number;

    ///
    /// \brief The condition variable is used to tell a worker that
    /// the queue is available.
    ///
    std::condition_variable     m_job_available;

    ///
    /// \brief This condition variable is notified when a thread finish a task.
    ///
    std::condition_variable     m_job_finished;

    ///
    /// \brief This mutex is used to access the threadpool safely by workers.
    ///
    std::mutex  m_mutex;

    ///
    /// \brief Queue of task that should be proceeded by the thread pool.
    ///
    /// We use a queue for the FIFO (first-in, first-out) concept.
    ///
    std::queue<std::function<void()>>   m_jobs;

    ///
    /// \brief Workers used by the thread pool.
    ///
    std::vector<std::thread>    m_workers;
};
