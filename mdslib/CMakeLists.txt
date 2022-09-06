
###
### MdsLib
###

ADD_LIBRARY(
    MdsLib SHARED
    MdsLib.c
)

INSTALL(TARGETS MdsLib)

TARGET_LINK_LIBRARIES(
    MdsLib
    PUBLIC
        TdiShr
)

ADD_STATIC_COPY(MdsLib)

###
### MdsLib_client
###

ADD_LIBRARY(
    MdsLib_client SHARED
    MdsLib.c
)

INSTALL(TARGETS MdsLib_client)

TARGET_LINK_LIBRARIES(
    MdsLib_client
    PUBLIC
        TdiShr
)

TARGET_COMPILE_DEFINITIONS(
    MdsLib_client
    PRIVATE
        -D_CLIENT_ONLY
)

ADD_STATIC_COPY(MdsLib_client)

###
### MdsLib_fortran
###

ADD_LIBRARY(
    MdsLib_fortran SHARED
    MdsLib.c
)

INSTALL(TARGETS MdsLib_fortran)

TARGET_LINK_LIBRARIES(
    MdsLib_fortran
    PUBLIC
        TdiShr
)

TARGET_COMPILE_DEFINITIONS(
    MdsLib_fortran
    PRIVATE
        -DOLD_FORTRAN_API
)

ADD_STATIC_COPY(MdsLib_fortran)
