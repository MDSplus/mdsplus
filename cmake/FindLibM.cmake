# FindLibM.cmake
#
# Finds libm, if present on this system, otherwise creates an empty target
#
# This will define the following variables
#
#   LibM_FOUND
#   LibM_LIBRARIES
#
# and the following imported targets
#
#   LibM::LibM
#

set(LibM_FOUND TRUE)

find_library(
    LibM_LIBRARIES
    NAMES m
)

mark_as_advanced(
    LibM_FOUND
    LibM_LIBRARIES
)

if(NOT TARGET LibM::LibM)

    add_library(LibM::LibM INTERFACE IMPORTED)

    if(LibM_LIBRARIES)

        set_target_properties(
            LibM::LibM
            PROPERTIES
                INTERFACE_LINK_LIBRARIES "${LibM_LIBRARIES}"
        )

    endif()

endif()