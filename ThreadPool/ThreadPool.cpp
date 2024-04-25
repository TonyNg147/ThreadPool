#include "ThreadPool.h"
#include <condition_variable>
#include <memory>
#include <iostream>
#include <thread>
namespace Util
{
    Task::Task(std::function<void()> function, ThreadPool* pool, const std::shared_ptr<Task::TaskState>& state)
    {
        if (pool == nullptr)
        {
            throw std::runtime_error("Need to specify a Valid Thread pool");
        }

        if (function == nullptr)
        {
            throw std::runtime_error("Ignore the invalid function request");
        }

        m_pool = pool;

        m_function = function;

        if (state)
        {
            m_state = state;
        }
        else
        {
            m_state = std::make_shared<TaskState>();
        }
    }

    void Task::start()
    {
        if (m_pool == nullptr) return;

        m_state->tasks.push_back(m_function);

        if (m_state->worker)
        {
            m_state->condState.notify_one();
        }
        else
        {
            std::shared_ptr<TaskState> state = m_state;
            
            std::condition_variable& pooling_condition = m_pool->m_condition;

            ThreadPool* pooling = m_pool;

            m_state->isConductingTask = true;

            m_state->worker = new std::thread([state, pooling, &pooling_condition]()-> void{

                while(state->isConductingTask)
                {

                    std::cout<<"This thread is " << std::this_thread::get_id() << "\n";
                    
                    std::vector<std::function<void()>>::iterator it = state->tasks.begin();

                    // Loop for all the functions in queue. This can be optimized by ony 
                    // allowing one instance of function call

                    while(it != state->tasks.end())
                    {
                        (*it)();
                        ++it;
                    }

                    std::unique_lock<std::mutex> lock(state->lockState);
                    if (state->tasks.size())
                    {
                        state->tasks.erase(state->tasks.begin());
                    }
                    
                    // Whenever the executions are done, we wait to be awakend in 2 scenarios
                    // 1. If there is a new function call is pushed into the executions
                    // 2. Timeout event is triggered. That means no need for the existance of this thread
                    //    we notify to pool manager and release it 

                    state->condState.wait_for(lock,std::chrono::seconds(5), [state]()-> bool {return state->tasks.size();});

                    std::cout<<"Wake up after 5 seconds. Notify " << state->tasks.size() << "\n";

                    if (state->tasks.size() == 0)
                    {
                        state->isConductingTask = false;

                        std::cout<<"Call notify\n";

                        pooling_condition.notify_one();

                        state->condState.wait(lock);
                    }
                };
            });

            pooling->AddTask(m_state);

        }
    }

    ThreadPool& ThreadPool::getInstance()
    {
        static ThreadPool instance;
        return instance;
    }

    ThreadPool::ThreadPool()
    {
        m_isRunningRoutine = true;
    }

    void ThreadPool::AddTask(const std::shared_ptr<Task::TaskState>& taskState)
    {
        if (!taskState) return;

        std::unique_lock<std::mutex> lock(m_mutex);

        m_stateControls.push_back(taskState);

    }

    ThreadPool::~ThreadPool()
    {
        m_isRunningRoutine = false;
    }

    Task ThreadPool::createTask(std::function<void()> function)
    {
        if (function == nullptr)
        {
            throw std::runtime_error("Cannot create an invalid task");
        }

        std::unique_lock<std::mutex> lock(m_mutex);

        std::list<std::shared_ptr<Task::TaskState>>::iterator it = m_stateControls.begin();

        std::shared_ptr<Task::TaskState> availableState;

        // Check all the threads in Pool and determine the empty-task one
        // If there is no applicant, a new one would be created

        while(it != m_stateControls.end())
        {
            if ((*it) && (*it)->tasks.size() == 0)
            {
                availableState = (*it);
                break;
            }
            ++it;
        }

        return Task(function, this, availableState);
    }

    void ThreadPool::doRoutine()
    {
        std::thread worker([this]() -> void 
        {
            while(m_isRunningRoutine)
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                
                m_condition.wait(lock);  // the condition will wait until there is a request to release a thread

                auto it = m_stateControls.begin();

                while(it != m_stateControls.end())
                {
                    if (static_cast<bool>(*it))
                    {
                        std::cout<<"Check if not conduct Task " << (*it)->isConductingTask << "\n";
                        if (!(*it)->isConductingTask)
                        {

                            (*it)->condState.notify_all();

                            (*it)->worker->join();

                            delete (*it)->worker;

                            m_stateControls.erase(it++);
                        }
                        else
                        {
                            ++it;
                        }
                    }
                    else
                    {
                        m_stateControls.erase(it++);
                    }
                }

                lock.unlock();
            }
        });

        worker.detach();

    }

};

