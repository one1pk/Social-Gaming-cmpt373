<<<<<<< HEAD
cmake_minimum_required(VERSION 3.12)
project(SocialGaming)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG 
    "-O0 -g -fsanitize=address,undefined -fno-omit-frame-pointer"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

add_subdirectory(lib)
add_subdirectory(tools)

=======

cmake_minimum_required(VERSION 3.2)

project(Interpreter)

find_package(nlohmann_json 3.2.0 REQUIRED)
add_executable(interpret interpretJson.cpp interpret.h)
target_link_libraries(interpret PRIVATE nlohmann_json::nlohmann_json)

set_target_properties(interpret PROPERTIES CXX_STANDARD 17)
>>>>>>> 3cc5f6f (Add interpreter definition and cmake for external library)
