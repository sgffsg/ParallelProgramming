cmake_minimum_required(VERSION 3.31)
project(lw1)

set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(
        "../googletest"
        "${CMAKE_BINARY_DIR}/googletest"
)

add_executable(${PROJECT_NAME}
        main.cpp
)