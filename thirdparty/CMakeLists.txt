set(THIRDPARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# -----------------------------
# GLFW
# -----------------------------
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(glfw)


# -----------------------------
# ENet options
# -----------------------------
set(ENET_STATIC ON CACHE BOOL "" FORCE)
set(ENET_SHARED OFF CACHE BOOL "" FORCE)
set(ENET_TEST OFF CACHE BOOL "" FORCE)

add_subdirectory(enet)


# -----------------------------
# Raylib options
# -----------------------------
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(USE_EXTERNAL_GLFW ON CACHE BOOL "" FORCE)
add_subdirectory(raylib)


add_subdirectory(imgui)
add_subdirectory(rlimgui)
