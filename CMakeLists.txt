cmake_minimum_required(VERSION 3.0.0)
project(ThreadPool VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)

set(SOURCE_FILE
    main.cpp 
    ThreadPool.cpp)

find_package(Threads)

add_executable(ThreadPool ${SOURCE_FILE})

target_link_libraries(ThreadPool ${CMAKE_THREAD_LIBS_INIT})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
