///
/// \file ThreadPool.hpp
///
/// \author Martin-Pierrat Louis (mart_p)
///
/// \date Mon, 18 Jan 2016 15:10:49
///
/// \version 2.2.18
///

#ifndef     __CSE_160_THREAD_POOL_HPP__
# define    __CSE_160_THREAD_POOL_HPP__

#include    <list>
#include    <queue>
#include    <functional>
#include    <thread>
#include    <mutex>
#include    <condition_variable>

namespace   CSE160
{
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
            /// Part of the rule of three, more information can be found here :
            /// https://en.wikipedia.org/wiki/Rule_of_three_%28C%2B%2B_programming%29#Rule_of_Three
            ///
            /// We must delete the copy constructor because of the non copyable
            /// type of a std::condition_variable.
            ///
            ThreadPool(const ThreadPool &other) = delete;

            ///
            /// \brief Move constructor.
            ///
            /// Part of the rule of five, more information can be found here :
            /// https://en.wikipedia.org/wiki/Rule_of_three_%28C%2B%2B_programming%29#Rule_of_Five
            ///
            /// We must delete the move constructor because of the non movable type
            /// of a std::condition_variable.
            ///
            ThreadPool(ThreadPool &&other) noexcept = delete;

            ///
            /// \brief Destructor.
            ///
            /// Part of the rule of three, more information can be found here :
            /// https://en.wikipedia.org/wiki/Rule_of_three_%28C%2B%2B_programming%29#Rule_of_Three
            ///
            virtual ~ThreadPool(void) noexcept;

        public:
            ///
            /// \brief Copy assignment operator.
            ///
            /// Part of the rule of three, more information can be found here :
            /// https://en.wikipedia.org/wiki/Rule_of_three_%28C%2B%2B_programming%29#Rule_of_Three
            ///
            /// We must delete the copy assignment operator because of the
            /// non copyable type of a std::condition_variable.
            ///
            ThreadPool  &operator=(const ThreadPool &other) = delete;

            ///
            /// \brief Move assignment operator.
            ///
            /// Part of the rule of five, more information can be found here :
            /// https://en.wikipedia.org/wiki/Rule_of_three_%28C%2B%2B_programming%29#Rule_of_Five
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
            /// More information about std::function can be found here :
            /// http://en.cppreference.com/w/cpp/utility/functional/function
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
            bool                                terminate;

            ///
            /// \brief Number of workers actually processing a job.
            ///
            std::size_t                         busy_workers_number;

            ///
            /// \brief The condition variable is used to tell a worker that
            /// the queue is available.
            ///
            /// More informations available here :
            /// http://en.cppreference.com/w/cpp/thread/condition_variable
            ///
            std::condition_variable             job_available;

            ///
            /// \brief This condition variable is notified when a thread finish a task.
            ///
            /// More informations available here :
            /// http://en.cppreference.com/w/cpp/thread/condition_variable
            ///
            std::condition_variable             job_finished;

            ///
            /// \brief This mutex is used to access the threadpool safely by workers.
            ///
            /// More information available here :
            /// http://en.cppreference.com/w/cpp/thread/mutex
            ///
            std::mutex                          mutex;

            ///
            /// \brief Queue of task that should be proceeded by the thread pool.
            ///
            /// We use a queue for the FIFO (first-in, first-out) concept.
            ///
            /// More informations about queue can be found here :
            /// http://en.cppreference.com/w/cpp/container/queue
            ///
            std::queue<std::function<void()>>   jobs;

            ///
            /// \brief Workers used by the thread pool.
            ///
            /// We can use something else than a list in this case, but a threadpool
            /// should implement a "add_workers" method. it's is not available
            /// in this basic implementation.
            ///
            std::list<std::thread>              workers;
    };

}

#endif /* !__CSE_160_THREAD_POOL_HPP__ */
