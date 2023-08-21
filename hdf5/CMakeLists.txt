
if(HDF5_FOUND)

    compile_definitions(
        -DH5_USE_16_API
    )

    ###
    ### hdf5tdi
    ###

    add_library(
        hdf5tdi
        hdf5tdi.c
    )

    target_link_libraries(
        hdf5tdi
        PUBLIC
            TdiShr
            HDF5::HDF5
    )

    mdsplus_add_static_copy(hdf5tdi)

    install(TARGETS hdf5tdi hdf5tdi-static)

    ###
    ### hdf5ToMds
    ###

    add_executable(
        hdf5ToMds
        hdf5ToMds.c
    )

    target_link_libraries(
        hdf5ToMds
        PUBLIC
            TdiShr
            HDF5::HDF5
    )

    install(TARGETS hdf5ToMds)

    ###
    ### MDSplus2HDF5
    ###

    add_executable(
        MDSplus2HDF5
        MDSplus2HDF5.c
    )

    target_link_libraries(
        MDSplus2HDF5
        PUBLIC
            TdiShr
            HDF5::HDF5
    )

    install(TARGETS MDSplus2HDF5)

endif()