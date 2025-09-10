# FindLibFFI.cmake
#
# Finds libLibFFI
#
# This will define the following variables
#
#   LibFFI_FOUND
#   LibFFI_INCLUDE_DIRS
#   LibFFI_LIBRARIES
#
# and the following imported targets
#
#   LibFFI::LibFFI
#
# The following variables can be set as arguments
#
#   LibFFI_ROOT
#

find_package(PkgConfig QUIET)

pkg_check_modules(_LibFFI_PC QUIET ffi)

find_path(
    LibFFI_INCLUDE_DIRS
    NAMES ffi.h
    HINTS
        ${_LibFFI_PC_INCLUDE_DIRS}
    PATH_SUFFIXES
        include
        include/ffi
)

find_library(
    LibFFI_LIBRARIES
    NAMES ffi
    HINTS
        ${_LibFFI_PC_LIBRARY_DIRS}
    PATH_SUFFIXES
        lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    LibFFI
    REQUIRED_VARS
        LibFFI_LIBRARIES
        LibFFI_INCLUDE_DIRS
)

if(LibFFI_FOUND)

    if(NOT TARGET LibFFI::LibFFI)

        add_library(LibFFI::LibFFI INTERFACE IMPORTED)

        set_target_properties(
            LibFFI::LibFFI
            PROPERTIES
                INTERFACE_LINK_LIBRARIES "${LibFFI_LIBRARIES}"
                INTERFACE_INCLUDE_DIRECTORIES "${LibFFI_INCLUDE_DIRS}"
        )

    endif()

endif()

mark_as_advanced(
    LibFFI_INCLUDE_DIRS
    LibFFI_LIBRARIES
)
