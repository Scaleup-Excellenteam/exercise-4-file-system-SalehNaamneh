cmake_minimum_required(VERSION 3.10)

# Set the project name
project(MyFS)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(myfs myfs_main.cpp blkdev.cpp myfs.cpp vfs.cpp)

# Specify the include directories
target_include_directories(myfs PRIVATE ${PROJECT_SOURCE_DIR})
