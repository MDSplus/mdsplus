
add_subdirectory(backends/check)

###
### MdsTest
###

add_library(
    MdsTest STATIC
    base_backend.c
)

target_include_directories(
    MdsTest
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(
    MdsTest
    PUBLIC
        -D_TESTING
)

set_target_properties(
    MdsTest
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

if(APPLE)
    target_compile_definitions(
        MdsTest
        PUBLIC
            _DARWIN_C_SOURCE
    )
endif()

###
### MdsTestBackend
###

add_library(
    MdsTestBackend STATIC
    check_backend.c
)

target_include_directories(
    MdsTestBackend
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/check
)

target_compile_definitions(
    MdsTestBackend
    PUBLIC
        -D_TESTING
)

set_target_properties(
    MdsTestBackend
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
)

if(APPLE)
    target_compile_definitions(
        MdsTestBackend
        PUBLIC
            _XOPEN_SOURCE=700
    )
endif()

###
### MdsTestShr
###

# TODO: Improve

add_library(
    MdsTestShr
    $<TARGET_OBJECTS:Check>
    $<TARGET_OBJECTS:MdsTestBackend>
)

target_compile_definitions(
    MdsTestShr
    PUBLIC
        -D_TESTING
)

target_link_libraries(
    MdsTestShr
    PUBLIC
        # Link to get the links/options/definitions
        Check
        MdsTestBackend
)

if(APPLE)
    target_compile_definitions(
        MdsTestShr
        PUBLIC
            _DARWIN_C_SOURCE
    )
endif()
