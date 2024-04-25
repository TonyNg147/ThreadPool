project(ThreadPool)


set(SOURCES
ThreadPool/ThreadPool.h
ThreadPool/ThreadPool.cpp
)

add_executable(ThreadPool main.cpp ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ThreadPool)