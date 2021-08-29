# CMakeList.txt : CMake project for The Elder Tongue, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project (the-elder-tongue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BASE_SRCS
    main.cpp
    shader.h
    shader.cpp
    stb_image.h
    stb_image.cpp
    game.h
    game.cpp
    texture_2D.cpp
    texture_2D.h
    renderer.h
    renderer.cpp)

# Add source to this project's executable.
add_executable (the-elder-tongue ${BASE_SRCS} "check_error.h" "check_error.cpp")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw-3.3.4)
add_subdirectory(libs/glad)
add_subdirectory(libs/glm-0.9.9.8)

if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
     set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
endif()

target_link_libraries(the-elder-tongue glfw glad glm)
set_target_properties(the-elder-tongue PROPERTIES CXX_EXTENSIONS OFF)
