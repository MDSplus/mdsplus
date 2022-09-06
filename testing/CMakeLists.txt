
ADD_SUBDIRECTORY(backends/check)

###
### MdsTest
###

ADD_LIBRARY(
    MdsTest STATIC
    base_backend.c
)

TARGET_INCLUDE_DIRECTORIES(
    MdsTest
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

TARGET_COMPILE_DEFINITIONS(
    MdsTest
    PUBLIC
        -D_TESTING
)

SET_TARGET_PROPERTIES(
    MdsTest
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

###
### MdsTestBackend
###

ADD_LIBRARY(
    MdsTestBackend STATIC
    check_backend.c
)

TARGET_INCLUDE_DIRECTORIES(
    MdsTestBackend
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/check
)

TARGET_COMPILE_DEFINITIONS(
    MdsTestBackend
    PUBLIC
        -D_TESTING
)

SET_TARGET_PROPERTIES(
    MdsTestBackend
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

###
### MdsTestShr
###

# TODO: Improve

ADD_LIBRARY(
    MdsTestShr SHARED
    $<TARGET_OBJECTS:Check>
    $<TARGET_OBJECTS:MdsTestBackend>
)

TARGET_COMPILE_DEFINITIONS(
    MdsTestShr
    PUBLIC
        -D_TESTING
)

TARGET_LINK_LIBRARIES(
    MdsTestShr
    PUBLIC
        # Link to get the links/options/definitions
        Check
        MdsTestBackend
)
