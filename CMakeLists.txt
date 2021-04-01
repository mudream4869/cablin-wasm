project (client)
cmake_minimum_required(VERSION 3.5.1)
option(JS_ONLY "Compiles to native JS (No WASM)" OFF)

add_definitions(-std=c++17)

add_subdirectory(third_party/cablin)
include_directories(third_party/cablin/include)

file(GLOB SOURCES src/main.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/public")
add_executable(cablin_wasm ${SOURCES})
target_link_libraries(cablin_wasm cablin)

message(STATUS "Setting compilation target to WASM")

set_target_properties(cablin_wasm
    PROPERTIES
    LINK_FLAGS "-s WASM=0 -s EXPORTED_FUNCTIONS=['_cablinRun','_getCablinError'] -s EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap','UTF8ToString','addFunction','removeFunction'] -s ALLOW_TABLE_GROWTH -fexceptions")

target_compile_options(cablin_wasm PRIVATE -fexceptions)

