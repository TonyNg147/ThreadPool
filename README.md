# Thread Pool

## What is Thread
- Thread of execution (Thread for short) is a trackable function call (can determine how many functions has been invoked) and variable allocation (release the resources taken whenever the variable's scope is gone)

- All program starts with a Thread called Main Thread (In an GUI application is also called GUI Thread). 

- When the stack's origin of that thread is recalled. That means the thread comes in finish

## How thread works with STD
- The standard library (STD) introduce the std::thread with 2 operation mode **join** and **detach**
#### With Join
the thread which invokes other thread **has to wait for the spawned thread to release the Stack Frame** that has been allocated for that new thread whenever the thread reaches its end point. 
#### 
With detach

- we separate the thread of executions from the thread object. Allowing thread to continue running independently. Any allocated resources will be freed once the thread exist. With this approach, we solve the demand of unblocking the Main thread or invoking thread but the resources is so hard to release because the OS needs the thread ends its execution to do it


## Thread pool

In this project, i created a thread pool for the sake of detaching the execution from main thread as well as release and reuse whenever necessary. Then the Thread won't be created each time we request a new one. It is only created once all the threads are taken or no threads in pool