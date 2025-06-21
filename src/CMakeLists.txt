cmake_minimum_required(VERSION 3.6)
cmake_policy(VERSION 3.6...3.10)

project(JustDrive
        VERSION 0.1
        LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/include_all)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/assimp/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glm/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glad/)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/stbi/)

add_library(engine Engine/Mesh.cpp Engine/Model.cpp Engine/Shader.cpp)
#target_include_directories(engine BEFORE INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/external/assimp/include)
#target_include_directories(engine BEFORE INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include)
#target_include_directories(engine BEFORE INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/external/glm/include)
#target_include_directories(engine BEFORE INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include)
target_link_libraries(engine PUBLIC assimp::assimp)
target_link_libraries(engine PUBLIC glfw)
target_link_libraries(engine PUBLIC glad)
target_link_libraries(engine PUBLIC glm)
target_link_libraries(engine PUBLIC stbi)


add_library(jd Game/main.cpp Game/GameObject.cpp Game/Components.cpp Game/Camera.cpp Game/Car.cpp)
target_link_libraries(jd PUBLIC engine)

##target_link_libraries(jd INTERFACE assimp::assimp)
##target_link_libraries(jd INTERFACE glfw)
##target_link_libraries(jd INTERFACE glm)



#target_link_libraries(my_test ${CMAKE_SOURCE_DIR}/../lib/libglfw3.a)
#target_link_libraries(my_test ${CMAKE_SOURCE_DIR}/../lib/libassimp.a)

