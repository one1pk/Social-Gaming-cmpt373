add_library(interpreter
    src/InterpretJson.cpp
)
add_definitions(-DPATH_TO_JSON="${CMAKE_CURRENT_SOURCE_DIR}/../../tools/gameserver/gameconfigs/")

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(glog 0.4.0 REQUIRED)

target_include_directories(interpreter
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(interpreter
        nlohmann_json::nlohmann_json
        AST
        game  
        glog::glog
)

set_target_properties(interpreter
                    PROPERTIES
                    LINKER_LANGUAGE CXX
                    CXX_STANDARD 17
)

install(TARGETS interpreter
    ARCHIVE DESTINATION lib
)
