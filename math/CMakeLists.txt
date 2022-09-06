
###
### MdsMath
###

FILE(GLOB
    _source_list
    "*.f"
)

IF(LIBBLAS)
    FILE(GLOB
        _blas_source_list
        "blas/*.f"
    )
ENDIF()

ADD_LIBRARY(
    MdsMath SHARED
    ${_source_list}
    ${_blas_source_list}
)

INSTALL(
    TARGETS MdsMath
)

TARGET_LINK_LIBRARIES(
    MdsMath
    PUBLIC
        ${LIBBLAS}
)

# (SLW) TODO: Check compatible fortran?
TARGET_COMPILE_OPTIONS(
    MdsMath
    PRIVATE
        -Wno-maybe-uninitialized
        -Wno-compare-reals
)
