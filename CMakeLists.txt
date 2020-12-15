cmake_minimum_required(VERSION 3.17)
project(core)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(scheduler)

add_executable(core
        scheduler/scheduler-v1.0.0.cpp
        scheduler/scheduler-v1.0.0.h
        scheduler/task-queue-1.0.0.cpp
        scheduler/task-queue-1.0.0.h
        core.ino
        LICENSE
        README.md)
