add_library(serverstate
    src/globalState.cpp
)
add_definitions(-DPATH_TO_JSON="${CMAKE_CURRENT_SOURCE_DIR}/../../tools/gameserver/gameconfigs")

target_include_directories(serverstate
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(serverstate
  PUBLIC
    networking
    game
    interpreter
)

set_target_properties(serverstate
                    PROPERTIES
                    LINKER_LANGUAGE CXX
                    CXX_STANDARD 17
)

install(TARGETS serverstate
    ARCHIVE DESTINATION lib
)