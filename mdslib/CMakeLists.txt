
###
### MdsLib
###

add_library(
    MdsLib
    MdsLib.c
)

install(TARGETS MdsLib)

target_link_libraries(
    MdsLib
    PUBLIC
        TdiShr
)

add_static_copy(MdsLib)

###
### MdsLib_client
###

add_library(
    MdsLib_client
    MdsLib.c
)

install(TARGETS MdsLib_client)

target_link_libraries(
    MdsLib_client
    PUBLIC
        TdiShr
)

target_compile_definitions(
    MdsLib_client
    PRIVATE
        -D_CLIENT_ONLY
)

add_static_copy(MdsLib_client)

###
### MdsLib_fortran
###

add_library(
    MdsLib_fortran
    MdsLib.c
)

install(TARGETS MdsLib_fortran)

target_link_libraries(
    MdsLib_fortran
    PUBLIC
        TdiShr
)

target_compile_definitions(
    MdsLib_fortran
    PRIVATE
        -DOLD_FORTRAN_API
)

add_static_copy(MdsLib_fortran)
