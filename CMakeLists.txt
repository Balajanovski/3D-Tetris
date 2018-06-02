cmake_minimum_required(VERSION 3.6)
project(3d-tetris CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake-modules")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_CXX_FLAGS "-Wall")

find_package(GLFW 3.0.0)
message("- GLFW Located")
include_directories(${GLFW_INCLUDE_DIR})

# Define no debug
# Comment this out if you want debug mode
add_definitions(-DNDEBUG)

if(GLFW_FOUND AND (GLFW_VERSION VERSION_EQUAL 3.0 OR GLFW_VERSION VERSION_GREATER 3.0))
    add_definitions( -DGLFW_VERSION_3 )
endif()

find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
    message("- OpenGL Located")
else()
    message("- error: OpenGL not Located")
endif()
include_directories(${OPENGL_HEADER_DIR})

find_package(Freetype2 REQUIRED)
if (FREETYPE2_FOUND)
    message("- Freetype2 Located")
else()
    message("- error: Freetype2 not Located")
endif()
include_directories(${FREETYPE2_INCLUDE_DIRS})

find_package(OpenAL REQUIRED)
if (OPENAL_FOUND)
    message("- OpenAL Located")
else()
    message("- error: OpenAL not Located")
endif()
include_directories(${OPENAL_INCLUDE_DIR})


set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Source)

configure_file(${PROJECT_SOURCE_DIR}/Util/root_directory.h.in ${PROJECT_SOURCE_DIR}/Util/root_directory.h)

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
        ${FREETYPE2_LIBRARIES}
        ${OPENAL_LIBRARY}
        )

target_include_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/Includes
        ${PROJECT_SOURCE_DIR}
        )