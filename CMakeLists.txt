cmake_minimum_required(VERSION 3.15)

# set the project name
project(vpathtracer)

# Disable building extras we won't need (pure C++ project)
set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_PYTHON  OFF CACHE BOOL " " FORCE)
set(NANOGUI_INSTALL       OFF CACHE BOOL " " FORCE)

# Add the configurations from nanogui
add_subdirectory(ext/nanogui)

# For reliability of parallel build, make the NanoGUI targets dependencies
set_property(TARGET glfw glfw_objects nanogui PROPERTY FOLDER "dependencies")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME}
    # headers
    include/common.h
    include/application.h
    include/renderer.h
    include/obj_loader.h
    include/scene.h
    include/object.h
    include/triangle.h
    include/ray.h

    # sources
    src/main.cpp
    src/application.cpp
    src/renderer.cpp
    src/scene.cpp
    src/object.cpp
    src/triangle.cpp
)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC nanogui Threads::Threads)
target_include_directories(${PROJECT_NAME} PUBLIC
    ext/nanogui/include
    ext/nanogui/ext/nanovg/src
    ext/nanogui/ext/nanovg/example
    ext/glm/include
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)