
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
        TdiShr
    PRIVATE
        MdsIpShr
)

target_compile_definitions(
    MdsLib
    PRIVATE
        -DLIBPREFIX=MdsLib
)

mdsplus_add_static_copy(MdsLib)

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
        TdiShr
    PRIVATE
        MdsIpShr
)

target_compile_definitions(
    MdsLib_client
    PRIVATE
        -D_CLIENT_ONLY
)

mdsplus_add_static_copy(MdsLib_client)

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
        TdiShr
    PRIVATE
        MdsIpShr
)

target_compile_definitions(
    MdsLib_fortran
    PRIVATE
        -DOLD_FORTRAN_API
)

mdsplus_add_static_copy(MdsLib_fortran)

install(TARGETS MdsLib_fortran MdsLib_fortran-static)
