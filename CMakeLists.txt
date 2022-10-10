
cmake_minimum_required(VERSION 3.2)

project(Interpreter)

find_package(nlohmann_json 3.2.0 REQUIRED)
add_executable(interpret interpretJson.cpp interpret.h)
target_link_libraries(interpret PRIVATE nlohmann_json::nlohmann_json)

set_target_properties(interpret PROPERTIES CXX_STANDARD 17)