cmake_minimum_required(VERSION 3.6)
project(3d-tetris CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake-modules")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_CXX_FLAGS "-Wall")

find_package(GLFW 3.0.0)
message("- GLFW Located")
include_directories(${GLFW_INCLUDE_DIR})

if(GLFW_FOUND AND (GLFW_VERSION VERSION_EQUAL 3.0 OR GLFW_VERSION VERSION_GREATER 3.0))
    add_definitions( -DGLFW_VERSION_3 )
endif()

find_package(OpenGL REQUIRED)
message("- OpenGL Located")
include_directories(${OPENGL_INCLUDE_DIR})

set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Source)

file(GLOB SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/*.h"
        "${PROJECT_SOURCE_DIR}/*.cpp"

        "${PROJECT_SOURCE_DIR}/Includes/glad/*.h"
        "${PROJECT_SOURCE_DIR}/Includes/glad/*.c"

        "${PROJECT_SOURCE_DIR}/Includes/std_image/*.h"
        "${PROJECT_SOURCE_DIR}/Includes/stb_image/*.cpp"

        "${PROJECT_SOURCE_DIR}/Util/*.h"
        "${PROJECT_SOURCE_DIR}/Util/*.cpp"
        )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC
        ${GLFW_LIBRARIES}
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/Includes
        ${PROJECT_SOURCE_DIR}
        )