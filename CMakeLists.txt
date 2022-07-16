cmake_minimum_required(VERSION 3.5)
project(mesh-viewer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wno-deprecated-declarations -Wno-unused-function")

add_subdirectory(lib/nanogui)
include_directories(lib/nanogui/include)
include_directories(${NANOGUI_EXTRA_INCS})
include_directories(src)
add_definitions(${NANOGUI_EXTRA_DEFS})
add_subdirectory(lib/soil soil)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
#list(REMOVE_ITEM SOURCES "lib/nanogui/src/example*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC lib/soil/inc)
target_link_libraries(${PROJECT_NAME} nanogui ${NANOGUI_EXTRA_LIBS} soil)