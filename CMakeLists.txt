# ==========================================
# Root CMakeLists.txt
# Project structure:
#
# /solution_folder
#   /src
#       main.cpp
#   /thirdparty 
#       /enet
#       /glfw
#       /imgui
#       /raylib
#       /rlimgui
# ==========================================

cmake_minimum_required(VERSION 3.20)

project(mynet)
project(myservergui)
project(myhostgui)

set(ALL_TARGETS 
    mynet
    myservergui
    myclientgui
)
# -----------------------------
# Language standards
# -----------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------
# Visual Studio organization
# -----------------------------
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# -----------------------------
# Output directories
# -----------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# -----------------------------
# GLFW options
# -----------------------------
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# -----------------------------
# ENet options
# -----------------------------
set(ENET_STATIC ON CACHE BOOL "" FORCE)
set(ENET_SHARED OFF CACHE BOOL "" FORCE)
set(ENET_TEST OFF CACHE BOOL "" FORCE)

# -----------------------------
# Raylib options
# -----------------------------
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

# -----------------------------
# Dependencies
# -----------------------------
add_subdirectory(thirdparty)


# -----------------------------
# Main executable
# -----------------------------
add_executable(mynet
    src/main.cpp
    src/server/server.cpp
    src/client/client.cpp
)
add_executable(myservergui
    src/server/serverGUI.cpp
    src/server/server.cpp
)
add_executable(myclientgui
    src/client/clientGUI.cpp
    src/client/client.cpp
)

if(MSVC)
    foreach(target IN LISTS ALL_TARGETS)
        set_target_properties(${target} PROPERTIES
            WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>
        )
        target_link_options(${target} PRIVATE 
             $<$<NOT:$<CONFIG:Debug>>:/ENTRY:mainCRTStartup>
        )
    endforeach()
endif()

# -----------------------------
# Include directories
# -----------------------------
foreach(target IN LISTS ALL_TARGETS)
    target_include_directories(${target}
        PRIVATE
            src
    )
endforeach()

# -----------------------------
# Link libraries
# -----------------------------
foreach(target IN LISTS ALL_TARGETS)
    target_link_libraries(${target}
        PRIVATE
            #glfw
            raylib
            imgui
            enet
            rlimgui
    )
endforeach()

# -----------------------------
# Windows-specific system libs
# -----------------------------
if(WIN32)
    foreach(target IN LISTS ALL_TARGETS)
        target_link_libraries(${target}
            PRIVATE
                opengl32
                ws2_32
                winmm
                gdi32
        )
    endforeach()
endif()

# -----------------------------
# Linux system libs
# -----------------------------
if(UNIX AND NOT APPLE)
    foreach(target IN LISTS ALL_TARGETS)
        target_link_libraries(${target}
            PRIVATE
                m
                pthread
                dl
                rt
                X11
        )
        endforeach()
endif()

# -----------------------------
# Compiler warnings
# -----------------------------
if(MSVC)
    foreach(target IN LISTS ALL_TARGETS)
        target_compile_options(${target} PRIVATE /W4)
    endforeach()
else()
    foreach(target IN LISTS ALL_TARGETS)
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endforeach()
endif()

# -----------------------------
# Visual Studio startup project
# -----------------------------
foreach(target IN LISTS ALL_TARGETS)
    set_property(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY VS_STARTUP_PROJECT ${target}
    )
endforeach()