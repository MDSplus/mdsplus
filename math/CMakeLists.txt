
###
### MdsMath
###

file(GLOB
    _source_list
    "*.f"
    "*.c"
)

add_library(
    MdsMath SHARED
    ${_source_list}
)

install(
    TARGETS MdsMath
)

IF(LIBBLAS)

    target_link_libraries(
        MdsMath
        PUBLIC
            ${LIBBLAS}
    )

ELSE()

    message("Unable to find libblas, using local copy")

    file(GLOB
        _blas_source_list
        "blas/*.f"
    )

    target_sources(
        MdsMath
        PRIVATE
            ${_blas_source_list}
    )

ENDIF()

# (SLW) TODO: Check compatible fortran?
target_compile_options(
    MdsMath
    PRIVATE
        $<$<COMPILE_LANGUAGE:Fortran>:
            -Wno-maybe-uninitialized
            -Wno-compare-reals
        >
)