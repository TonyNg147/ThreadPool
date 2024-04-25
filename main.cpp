#include <chrono>
#include <iostream>

#include <ThreadPool.h>
#include <thread>


// In this example, I set the default timeout for thread existance to 5s
// If you request a thread for an execution within 5s, and after a previous call is done
//    you will see that the Thread ID remains unchanged
// In other cases, the thread ID is different from the previous one indicating that new thread has been created

int main()
{

    Util::ThreadPool::getInstance().doRoutine();
    Util::Task task = Util::ThreadPool::getInstance().createTask([](){
        std::cout<<"Call thread 1\n";
    });

    task.start();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    Util::Task task1 = Util::ThreadPool::getInstance().createTask([](){
        std::cout<<"Call thread 2\n";
    });

    task1.start();

    while(1);
}