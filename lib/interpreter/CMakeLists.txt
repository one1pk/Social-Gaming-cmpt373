add_library(interpreter
    src/InterpretJson.cpp
)

find_path(CHAISCRIPT_INCLUDE_DIRS "chaiscript/chaiscript.hpp")

target_include_directories(interpreter
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CHAISCRIPT_INCLUDE_DIRS}
)

target_link_libraries(interpreter
        game
)

set_target_properties(interpreter
                    PROPERTIES
                    LINKER_LANGUAGE CXX
                    CXX_STANDARD 17
)

install(TARGETS interpreter
    ARCHIVE DESTINATION lib
)
