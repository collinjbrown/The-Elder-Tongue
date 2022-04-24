# CMakeList.txt : CMake project for The Moonlight Tongue, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project (the-moonlight-tongue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BASE_SRCS
    animation_2D.cpp
    animation_2D.h
    check_error.cpp
    check_error.h
    component.h
    particleengine.h
    ecs.h
    entity.h
    game.cpp
    game.h
    main.cpp
    main.h
    renderer.cpp
    renderer.h
    shader.cpp
    shader.h
    external/stb_image.cpp
    external/stb_image.h
    system.h
    texture_2D.cpp
    texture_2D.h
    )

# Add source to this project's executable.
add_executable (the-moonlight-tongue ${BASE_SRCS})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/copy-assets.cmake
)

add_dependencies(the-moonlight-tongue copy_assets)

add_subdirectory(libs/glfw-3.3.4)
add_subdirectory(libs/glad)
add_subdirectory(libs/glm-0.9.9.8)

if(NOT DEFINED CMAKE_SUPPRESS_DEVELOPER_WARNINGS)
     set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS 1 CACHE INTERNAL "No dev warnings")
endif()

target_link_libraries(the-moonlight-tongue glfw glad glm)
set_target_properties(the-moonlight-tongue PROPERTIES CXX_EXTENSIONS OFF)
