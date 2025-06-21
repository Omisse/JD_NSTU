cmake_minimum_required(VERSION 3.6)
cmake_policy(VERSION 3.6...3.10)

project(JustDrive
        VERSION 0.1
        LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/include_all)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/assimp/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glm/)

add_library(engine Engine/Mesh.cpp Engine/Model.cpp Engine/Shader.cpp Engine/stbi.cpp Engine/glad.cpp)
target_link_libraries(engine PRIVATE assimp::assimp)
target_link_libraries(engine INTERFACE glfw)
target_link_libraries(engine INTERFACE glm)


add_library(jd Game/main.cpp Game/GameObject.cpp Game/Components.cpp Game/Camera.cpp Game/Car.cpp)
target_link_libraries(jd PRIVATE engine)
target_link_libraries(jd PRIVATE assimp::assimp)
target_link_libraries(jd PRIVATE glfw)
target_link_libraries(jd PRIVATE glm)



#target_link_libraries(my_test ${CMAKE_SOURCE_DIR}/../lib/libglfw3.a)
#target_link_libraries(my_test ${CMAKE_SOURCE_DIR}/../lib/libassimp.a)

