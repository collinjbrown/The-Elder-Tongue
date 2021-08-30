# CMakeList.txt : CMake project for The Elder Tongue, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project (the-elder-tongue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BASE_SRCS
    check_error.cpp
    check_error.h
    entity.h
    game.cpp
    game.h
    main.cpp
    main.h
    renderer.cpp
    renderer.h
    shader.cpp
    shader.h
    stb_image.cpp
    stb_image.h
    texture_2D.cpp
    texture_2D.h
    world.h
    )

# Add source to this project's executable.
add_executable (the-elder-tongue ${BASE_SRCS})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw-3.3.4)
add_subdirectory(libs/glad)
add_subdirectory(libs/glm-0.9.9.8)
add_subdirectory(libs/entt)

if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
     set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
endif()

target_link_libraries(the-elder-tongue glfw glad glm EnTT::EnTT)
set_target_properties(the-elder-tongue PROPERTIES CXX_EXTENSIONS OFF)
