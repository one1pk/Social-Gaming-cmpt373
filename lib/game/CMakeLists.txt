add_library(game
    src/game.cpp
    src/rules.cpp
    src/list.cpp
)

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
        networking
        interpreter
)

install(TARGETS game
    ARCHIVE DESTINATION lib
)

