cmake_minimum_required(VERSION 3.6)
cmake_policy(VERSION 3.6...3.10)

project(JD_NSTU
        VERSION 0.3
        LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(src ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CONFIG)

include_directories(${src}/src/external/include_all)

add_subdirectory(${src}/src/)

add_executable(JustDrive ${src}/src/Game/main.cpp)
target_link_libraries(JustDrive jd)