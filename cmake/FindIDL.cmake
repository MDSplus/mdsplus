# FindIDL.cmake
#
# Finds IDL's export.h
#
# This will define the following variables
#
#   IDL_FOUND
#   IDL_INCLUDE_DIRS
#
# and the following imported targets
#
#   IDL::IDL
#
# The following variables can be set as arguments
#
#   IDL_ROOT_DIR
#

find_path(
    IDL_INCLUDE_DIRS
    NAMES export.h
    PATHS 
        ${IDL_ROOT_DIR}
    PATH_SUFFIXES
        external
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    IDL
    REQUIRED_VARS # The first one is displayed in the message
        IDL_INCLUDE_DIRS
)

if(IDL_FOUND)

    if(NOT TARGET IDL::IDL)

        add_library(IDL::IDL INTERFACE IMPORTED)
        
        set_target_properties(
            IDL::IDL PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${IDL_INCLUDE_DIRS}"
        )

        if(APPLE)

            # Apple does not support weak linking by default
            set_target_properties(
                IDL::IDL PROPERTIES
                INTERFACE_LINK_OPTIONS "LINKER:-undefined,dynamic_lookup,-no_fixup_chains"
            )

        endif()

    endif()

endif()

mark_as_advanced(
    IDL_INCLUDE_DIRS
)
