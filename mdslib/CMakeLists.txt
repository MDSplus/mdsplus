
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

mdsplus_add_static_copy(MdsLib _static_target)

install(TARGETS MdsLib ${_static_target})

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

mdsplus_add_static_copy(MdsLib_client _static_target)

install(TARGETS MdsLib_client ${_static_target})

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

mdsplus_add_static_copy(MdsLib_fortran _static_target)

install(TARGETS MdsLib_fortran ${_static_target})
