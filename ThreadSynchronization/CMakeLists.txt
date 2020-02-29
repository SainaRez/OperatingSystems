cmake_minimum_required (VERSION 2.8.11)
set(CMAKE_C_STANDARD 11)
project (ThreadSynchronization)

add_executable(main.o main.c thread_demo.c thread_demo.h queue.c queue.h data.h)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(main.o m)

