# My File System Simulator

This project is a simple file system simulator implemented in C++. It simulates a virtual file system and block devices, allowing users to perform basic file system operations.

## Project Overview

The project includes the following components:

- **Block Device Layer (`blkdev.cpp` and `blkdev.h`):** 
  - Simulates a basic block device interface.
  - Handles read and write operations at the block level.

- **Virtual File System Layer (`vfs.cpp` and `vfs.h`):** 
  - Provides an abstraction over the block device.
  - Manages file and directory operations.

- **Main File System Logic (`myfs.cpp` and `myfs.h`):** 
  - Implements the core file system functions.
  - Manages metadata, file allocation, and directory structure.

- **Main Application (`myfs_main.cpp`):**
  - Entry point of the program.
  - Handles user input and interacts with the file system.

## Requirements

To build and run this project, you need the following:

- A C++ Compiler (such as `g++`)
- CMake (version 3.10 or higher)
- Make
- A Unix-like environment (Linux, macOS, or Windows with WSL)

## Building the Project

### Using CMake

1. **Navigate to the Project Directory:**

   ```bash
   cd /path/to/extracted/files

