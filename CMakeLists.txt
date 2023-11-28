cmake_minimum_required(VERSION 3.25)
project(IMS_Project)

set(CMAKE_CXX_STANDARD 17)

add_executable(IMS_Project
        main.cpp constants.h)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} m simlib)
