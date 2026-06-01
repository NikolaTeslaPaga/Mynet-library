# ==========================================
# Root CMakeLists.txt
# Project structure:
#
# /solution_folder
#   /src
#       /server
#           joinCode.h
#           joinCode.cpp
#           server.h
#           server.cpp
#           serverGUI.h
#           serverGUI.cpp
#       /client
#           client.h
#           client.cpp
#           clientGUI.h
#           clientGUI.cpp
#       enetGlobal.h
#       mynet.h
#       mynet.cpp
#   /thirdparty
#       /enet
#       /glfw
#       /imgui
#       /raylib
#       /rlimgui
# ==========================================

cmake_minimum_required(VERSION 3.20)

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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)



project(mynet)

if(MSVC)

    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    add_compile_definitions(
        _CRT_SECURE_NO_WARNINGS
        _WINSOCK_DEPRECATED_NO_WARNINGS
    )

endif()

set(ALL_TARGETS 
    #mynet
    myservergui
    myclientgui
)

add_subdirectory(thirdparty)

# -----------------------------
# Main executable
# -----------------------------

#add_executable(mynet
#    src/mynet.cpp
#    src/server/server.cpp
#    src/client/client.cpp
#    src/server/joinCode.cpp
#)

add_executable(myservergui
    src/server/serverGUI.cpp
    src/server/server.cpp
    src/server/joinCode.cpp
)
add_executable(myclientgui
    src/client/clientGUI.cpp
    src/client/client.cpp
    src/server/joinCode.cpp
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
            glfw
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