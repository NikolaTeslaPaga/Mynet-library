cmake_minimum_required(VERSION 3.20)
project(rlimgui)


set(RLIMGUI_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(rlimgui STATIC
    ${RLIMGUI_DIR}/rlImGui.cpp
)

target_include_directories(rlimgui PUBLIC
    ${THIRDPARTY_DIR}/imgui
    ${THIRDPARTY_DIR}/rlimgui
    ${THIRDPARTY_DIR}/raylib/src
)


target_link_libraries(rlimgui PUBLIC
    imgui
    raylib
)
