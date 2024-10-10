# FindDC1394.cmake
#
# Finds both dc1394 and raw1394 for use with IEEE1394 cameras
#
# This will define the following variables
#
#   DC1394_FOUND
#   DC1394_INCLUDE_DIRS
#   DC1394_dc1394_LIBRARY
#   DC1394_raw1394_LIBRARY
#   DC1394_LIBRARIES
#
# and the following imported targets
#
#   DC1394::DC1394
#
# The following variables can be set as arguments
#
#   DC1394_ROOT_DIR
#

find_path(
    DC1394_INCLUDE_DIRS
    NAMES dc1394/dc1394.h
    PATHS
        ${DC1394_ROOT_DIR}
    PATH_SUFFIXES
        include
)

find_library(
    DC1394_dc1394_LIBRARY
    NAMES dc1394
    PATHS
        ${DC1394_ROOT_DIR}
    PATH_SUFFIXES
        lib
)

find_library(
    DC1394_raw1394_LIBRARY
    NAMES raw1394
    PATHS
        ${DC1394_ROOT_DIR}
    PATH_SUFFIXES
        lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    DC1394
    REQUIRED_VARS # The first one is displayed in the message
        DC1394_dc1394_LIBRARY
        DC1394_INCLUDE_DIRS
        DC1394_raw1394_LIBRARY
)

if(DC1394_FOUND)

    set(DC1394_LIBRARIES
        ${DC1394_dc1394_LIBRARY}
        ${DC1394_raw1394_LIBRARY}
    )

    if(NOT TARGET DC1394::DC1394)

        add_library(DC1394::DC1394 INTERFACE IMPORTED)

        set_target_properties(
            DC1394::DC1394
            PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${DC1394_INCLUDE_DIRS}"
                INTERFACE_LINK_LIBRARIES "${DC1394_LIBRARIES}"
        )

    endif()
    
endif()

mark_as_advanced(
    DC1394_INCLUDE_DIRS
    DC1394_LIBRARIES
    DC1394_dc1394_LIBRARY
    DC1394_raw1394_LIBRARY
)
