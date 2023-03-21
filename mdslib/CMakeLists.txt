
###
### MdsLib
###

add_library(
    MdsLib
    MdsLib.c
)

target_link_libraries(
    MdsLib
    PUBLIC
        MdsIpShr
)

target_compile_definitions(
    MdsLib
    PRIVATE
        -DLIBPREFIX=MdsLib
)

add_static_copy(MdsLib)

install(TARGETS MdsLib MdsLib-static)

###
### MdsLib_client
###

add_library(
    MdsLib_client
    MdsLib.c
)

target_link_libraries(
    MdsLib_client
    PUBLIC
        MdsIpShr
)

target_compile_definitions(
    MdsLib_client
    PRIVATE
        -D_CLIENT_ONLY
)

add_static_copy(MdsLib_client)

install(TARGETS MdsLib_client MdsLib_client-static)

###
### MdsLib_fortran
###

add_library(
    MdsLib_fortran
    MdsLib.c
)

target_link_libraries(
    MdsLib_fortran
    PUBLIC
        MdsIpShr
)

target_compile_definitions(
    MdsLib_fortran
    PRIVATE
        -DOLD_FORTRAN_API
)

add_static_copy(MdsLib_fortran)

install(TARGETS MdsLib_fortran MdsLib_fortran-static)
