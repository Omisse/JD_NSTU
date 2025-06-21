cmake_minimum_required(VERSION 3.6)
cmake_policy(VERSION 3.6...3.10)

project(stbi)

add_library(stbi src/stbi.cpp)
target_include_directories(stbi BEFORE PUBLIC include/)