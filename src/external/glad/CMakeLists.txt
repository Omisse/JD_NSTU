cmake_minimum_required(VERSION 3.6)
cmake_policy(VERSION 3.6...3.10)

project(glad)

add_library(glad src/glad.c)
target_include_directories(glad BEFORE PUBLIC include/)
