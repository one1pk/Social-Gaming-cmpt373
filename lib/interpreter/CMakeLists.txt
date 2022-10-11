add_library(interpreter
    src/interpreter.cpp
)

find_package(nlohmann_json 3.2.0 REQUIRED)

target_include_directories(interpreter
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${nlohmann_json_INCLUDE_DIR}
)

target_link_libraries(interpreter
    PRIVATE 
        ${nlohmann_json_LIBRARIES}
)

set_target_properties(interpreter
                    PROPERTIES
                    LINKER_LANGUAGE CXX
                    CXX_STANDARD 17
)

install(TARGETS interpreter
    ARCHIVE DESTINATION lib
)
