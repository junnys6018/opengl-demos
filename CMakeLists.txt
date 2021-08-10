cmake_minimum_required(VERSION 3.16)
project(opengl-demos)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release") 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ---------- specify the C++ standard ----------

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ---------- Global Configuration Settings ----------

add_compile_definitions($<$<CONFIG:Debug>:CONFIGURATION_DEBUG>)
add_compile_definitions($<$<CONFIG:Release>:CONFIGURATION_RELEASE>)

add_compile_definitions($<$<PLATFORM_ID:Windows>:PLATFORM_WINDOWS>)
add_compile_definitions($<$<PLATFORM_ID:Linux>:PLATFORM_LINUX>)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:hiddenFriend-") # to make catch2 compile
else()
    message(FATAL_ERROR "Unknown Compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# ---------- Add Third Party Libraries ----------

# glad
add_subdirectory(vendor/glad)

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
set(GLFW_INSTALL OFF CACHE BOOL " " FORCE)
add_subdirectory(vendor/glfw)

# imgui
add_subdirectory(vendor/imgui)

# stb
add_subdirectory(vendor/stb_image)

# Move libraries to its own folder
set_property(TARGET glad glfw imgui stb_image PROPERTY FOLDER "vendor")

# ---------- Main Application ----------

file(GLOB_RECURSE SOURCES src/*)
add_executable(opengl-demos ${SOURCES})

target_link_libraries(opengl-demos PUBLIC 
    glad
    glfw
    imgui
    stb_image
)

target_include_directories(opengl-demos PUBLIC
    vendor/glad
    vendor/glfw/include
    vendor/imgui
    vendor/glm
    vendor/stb_image
    src
    src/utils
    src/utils/camera
)

# set working dir when debugging on visual studio
set_property(TARGET opengl-demos PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/")