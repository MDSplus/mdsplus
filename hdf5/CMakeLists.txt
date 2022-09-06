
###
### hdf5tdi
###

ADD_LIBRARY(
    hdf5tdi SHARED
    hdf5tdi.c
)

TARGET_LINK_LIBRARIES(
    hdf5tdi
    PUBLIC
        TdiShr
        ${LIBHDF5}
)

TARGET_COMPILE_DEFINITIONS(
    hdf5tdi
    PUBLIC
        -DH5_USE_16_API
)

ADD_STATIC_COPY(hdf5tdi)

###
### hdf5ToMds
###

ADD_EXECUTABLE(
    hdf5ToMds
    hdf5ToMds.c
)

TARGET_LINK_LIBRARIES(
    hdf5ToMds
    PUBLIC
        TdiShr
        ${LIBHDF5}
)

TARGET_COMPILE_DEFINITIONS(
    hdf5ToMds
    PUBLIC
        -DH5_USE_16_API
)

###
### MDSplus2HDF5
###

ADD_EXECUTABLE(
    MDSplus2HDF5
    MDSplus2HDF5.c
)

TARGET_LINK_LIBRARIES(
    MDSplus2HDF5
    PUBLIC
        TdiShr
        ${LIBHDF5}
)

TARGET_COMPILE_DEFINITIONS(
    MDSplus2HDF5
    PUBLIC
        -DH5_USE_16_API
)