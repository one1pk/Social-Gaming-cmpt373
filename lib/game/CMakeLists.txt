add_library(game
    src/game.cpp
    src/rules.cpp
    src/list.cpp
)

find_package(glog 0.4.0 REQUIRED)

target_include_directories(game
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

set_target_properties(game
                    PROPERTIES
                    LINKER_LANGUAGE CXX
                    CXX_STANDARD 17
)

target_link_libraries(game
        AST
        interpreter
        glog::glog
        networking
)

install(TARGETS game
    ARCHIVE DESTINATION lib
)

