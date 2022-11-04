add_library(commandProcessing
    src/commandHandler.cpp
    src/commands.cpp
    src/messageProcessor.cpp
    src/globalState.cpp
)

target_include_directories(commandProcessing
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(commandProcessing
  PUBLIC
    networking
    game
)

set_target_properties(commandProcessing
                    PROPERTIES
                    LINKER_LANGUAGE CXX
                    CXX_STANDARD 17
)

install(TARGETS commandProcessing
    ARCHIVE DESTINATION lib
)