cmake_minimum_required(VERSION 3.14)
project(6502_Emulator)

# Global settings and include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add subdirectories
add_subdirectory(6502Lib)
add_subdirectory(6502Test)