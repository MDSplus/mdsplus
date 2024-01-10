# FindHDF5.cmake
#
# Finds the HDF5 data format library
#
# This will define the following variables
#
#   HDF5_FOUND
#   HDF5_INCLUDE_DIRS
#   HDF5_LIBRARIES
#
# and the following imported targets
#
#   HDF5::HDF5
#
# The following variables can be set as arguments
#
#   HDF5_ROOT_DIR
#

find_package(PkgConfig QUIET)

pkg_check_modules(_HDF5_PC QUIET hdf5)

find_path(
    HDF5_INCLUDE_DIRS
    NAMES hdf5.h
    PATHS
        ${HDF5_ROOT_DIR}
        ${_HDF5_PC_INCLUDE_DIRS}
    PATH_SUFFIXES
        include
)

find_library(
    HDF5_LIBRARIES
    NAMES hdf5
    PATHS
        ${HDF5_ROOT_DIR}
        ${_HDF5_PC_LIBRARY_DIRS}
    PATH_SUFFIXES
        lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    HDF5
    REQUIRED_VARS # The first one is displayed in the message
        HDF5_LIBRARIES
        HDF5_INCLUDE_DIRS
)

if(HDF5_FOUND)

    if(NOT TARGET HDF5::HDF5)

        add_library(HDF5::HDF5 INTERFACE IMPORTED)

        set_target_properties(
            HDF5::HDF5
            PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${HDF5_INCLUDE_DIRS}"
                INTERFACE_LINK_LIBRARIES "${HDF5_LIBRARIES}"
        )

    endif()
    
endif()

mark_as_advanced(
    HDF5_INCLUDE_DIRS
    HDF5_LIBRARIES
)