# Specify the minimum CMAKE version required
cmake_minimum_required(VERSION 2.8)

# Your project's name
project(CSGOFullscreenMinimap)

# Add all files from the source/ folder to CMake
file(GLOB_RECURSE SRC
    "src/*.cpp"
    "src/*.h"
    "src/*.hpp"
)

# Define the executable
add_executable(${PROJECT_NAME} ${SRC})
include_directories(src/)

if (WIN32)
set(SFML_DIR deps/SFML/lib/cmake/SFML)
set(SFML_STATIC_LIBRARIES TRUE)
endif()

find_package(SFML 2.5 COMPONENTS system window graphics audio REQUIRED)

if (UNIX)
	find_library(OpenGL OpenGL REQUIRED)
endif()

add_library(imgui STATIC deps/imgui/imgui.cpp deps/imgui/imgui_demo.cpp deps/imgui/imgui_draw.cpp deps/imgui/imgui_widgets.cpp)
include_directories(deps/imgui)

target_link_libraries(${PROJECT_NAME} imgui sfml-network sfml-graphics sfml-window sfml-audio sfml-system)

if (UNIX)
	target_link_libraries(${PROJECT_NAME} ${OpenGL})
endif()
if (WIN32)
	target_link_libraries(${PROJECT_NAME} OpenGL32)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
		CXX_STANDARD 17
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
		)
