cmake_minimum_required(VERSION 3.25)
project(IMS_Project)

set(CMAKE_CXX_STANDARD 14)

add_executable(IMS_Project
        main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} m simlib)
