#include <memory>
#include <thread>
#include <condition_variable>
#include <list>
#include <vector>
#include <functional>
namespace Util
{
    class ThreadPool;

    class Task
    {
        friend class ThreadPool;
        private:
            struct TaskState
            {
                std::mutex lockState;
                bool isConductingTask = false;
                std::condition_variable condState;
                std::thread* worker = nullptr;
                std::vector<std::function<void()>> tasks;
            };

        private:
            Task(std::function<void()> function, ThreadPool* pool, const std::shared_ptr<TaskState>& state);

        public:

            void start();


        private:

            ThreadPool* m_pool = nullptr;

            std::function<void()> m_function;

            std::shared_ptr<TaskState> m_state;
    };

    class ThreadPool
    {
        friend class Task; // need to enhance by creating the parent class
        private:
            ThreadPool();

        public:
            static ThreadPool& getInstance();
            
            ~ThreadPool();
        
            void AddTask(const std::shared_ptr<Task::TaskState>&);

            void doRoutine();

            Task createTask(std::function<void()> function);
            
        private:
            void WorkerThread();
            std::list<std::shared_ptr<Task::TaskState>> m_stateControls;
            std::condition_variable m_condition;
            std::mutex m_mutex;
            bool m_isRunningRoutine = false;;
    };
};