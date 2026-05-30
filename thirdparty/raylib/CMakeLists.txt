cmake_minimum_required(VERSION 3.25)

project(raylib)

add_library(raylib STATIC
	src/rcore.c
    src/rshapes.c
    src/rtextures.c
    src/rtext.c
    src/rmodels.c
    src/raudio.c
    src/rglfw.c
)

target_include_directories(raylib PUBLIC
    src
    src/external
    src/external/glfw/include
)

target_compile_definitions(raylib PRIVATE PLATFORM_DESKTOP)


if(WIN32)
    target_link_libraries(raylib PUBLIC winmm gdi32 opengl32 shell32)
endif()