add_library(game
    src/game.cpp
    src/rules.cpp
    src/list.cpp
)

find_package(nlohmann_json 3.2.0 REQUIRED)

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
    PRIVATE
        networking
        interpreter
        nlohmann_json::nlohmann_json
)

install(TARGETS game
    ARCHIVE DESTINATION lib
)

