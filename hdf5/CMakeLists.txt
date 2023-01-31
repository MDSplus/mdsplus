
if(HDF5_FOUND)

    compile_definitions(
        -DH5_USE_16_API
    )

    ###
    ### hdf5tdi
    ###

    add_library(
        hdf5tdi SHARED
        hdf5tdi.c
    )

    install(TARGETS hdf5tdi)

    target_link_libraries(
        hdf5tdi
        PUBLIC
            TdiShr
            HDF5::HDF5
    )

    add_static_copy(hdf5tdi)

    ###
    ### hdf5ToMds
    ###

    add_executable(
        hdf5ToMds
        hdf5ToMds.c
    )

    install(TARGETS hdf5ToMds)

    target_link_libraries(
        hdf5ToMds
        PUBLIC
            TdiShr
            HDF5::HDF5
    )

    ###
    ### MDSplus2HDF5
    ###

    add_executable(
        MDSplus2HDF5
        MDSplus2HDF5.c
    )

    install(TARGETS MDSplus2HDF5)

    target_link_libraries(
        MDSplus2HDF5
        PUBLIC
            TdiShr
            HDF5::HDF5
    )

endif()