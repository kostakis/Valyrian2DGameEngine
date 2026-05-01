cmake_minimum_required(VERSION 3.21)
project(2DGameDemonstration LANGUAGES CXX)

add_subdirectory(Engine)

file(GLOB all_src_files
    src/*.hpp
    src/*.cpp
)

add_executable(2DGameDemonstration ${all_src_files})

target_include_directories(2DGameDemonstration PRIVATE src)
target_link_libraries(2DGameDemonstration PRIVATE Engine)

# Windows only 
set_target_properties(2DGameDemonstration PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
