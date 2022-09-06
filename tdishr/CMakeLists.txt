
###
### TdiShr
###

# Generated Sources

set(_generated_sources
    TdiYacc.c
    tdiyacc.h
    TdiYacc.output
    TdiHash.c
    TdiLex.c
    tdilex.h
)

set(_run_yylex_gen FALSE)
foreach(_filename IN LISTS _generated_sources)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_filename}")
        message("Missing mdsdcl/${_filename}, will run deploy/yylex.gen tdishr")
        set(_run_yylex_gen TRUE)
    endif()
endforeach()

if(_run_yylex_gen)
    # TODO: Convert yylex.gen to python
    message("Running deploy/yylex.gen tdishr")
    execute_process(
        COMMAND deploy/yylex.gen tdishr
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

file(GLOB
    _source_list
    "*.c"
)

add_library(
    TdiShr
    ${_source_list}
)

target_link_libraries(
    TdiShr
    PUBLIC
        TreeShr
)

target_compile_definitions(
    TdiShr
    PRIVATE
        -DLIBPREFIX=Tdi
)

add_static_copy(TdiShr)

install(TARGETS TdiShr TdiShr-static)