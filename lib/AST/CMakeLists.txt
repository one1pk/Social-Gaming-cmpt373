add_library(AST
    src/ExpressionResolver.cpp
    src/ExpressionTree.cpp
    src/TreePrinter.cpp
)

target_include_directories(AST
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(AST
    PRIVATE
        game
)

set_target_properties(AST
                    PROPERTIES
                    LINKER_LANGUAGE CXX
                    CXX_STANDARD 17
)

install(TARGETS AST
    ARCHIVE DESTINATION lib
)
