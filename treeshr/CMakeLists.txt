
###
### TreeShr
###

# Generated Sources

set(
    _generated_sources
    TreeFindNodeWild.c
)

set(_run_yylex_gen FALSE)
foreach(_filename IN LISTS _generated_sources)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_filename}")
        message("Missing mdsdcl/${_filename}, will run deploy/yylex.gen treeshr")
        set(_run_yylex_gen TRUE)
    endif()
endforeach()

if(_run_yylex_gen)
    # TODO: Convert yylex.gen to python
    message("Running deploy/yylex.gen treeshr")
    execute_process(
        COMMAND deploy/yylex.gen treeshr
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

file(GLOB
    _source_list
    "*.c"
)

add_library(
    TreeShr
    ${_source_list}
)

install(TARGETS TreeShr)

target_link_libraries(
    TreeShr
    PUBLIC
        MdsShr
)

add_static_copy(TreeShr)