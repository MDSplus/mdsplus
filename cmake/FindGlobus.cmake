# FindGlobus.cmake
#
# Finds the Globus Toolkit (or Grid Community Toolkit)
#
# This will define the following variables
#
#   Globus_FOUND
#   Globus_INCLUDE_DIRS
#   Globus_LIBRARIES
#   Globus_globus_common_LIBRARY
#   Globus_globus_gridmap_callout_error_LIBRARY
#   Globus_globus_gss_assist_LIBRARY
#   Globus_globus_gssapi_error_LIBRARY
#   Globus_globus_gssapi_gsi_LIBRARY
#   Globus_globus_xio_gsi_driver_LIBRARY
#   Globus_globus_xio_LIBRARY
#
# and the following imported targets
#
#   Globus::Globus
#
# The following variables can be set as arguments
#
#   Globus_ROOT_DIR
#

find_package(PkgConfig QUIET)

pkg_check_modules(
    _Globus_PC QUIET
        globus-common
        globus-gridmap-callout-error
        globus-gss-assist
        globus-gssapi-error
        globus-gssapi-gsi
        globus-xio-gsi-driver
        globus-xio
)

find_path(
    Globus_INCLUDE_DIRS
    NAMES globus_common.h
    PATHS 
        ${Globus_ROOT_DIR}
        ${_Globus_PC_INCLUDE_DIRS}
    PATH_SUFFIXES
        include
)

find_library(
    Globus_globus_common_LIBRARY
    NAMES globus_common
    PATHS 
        ${Globus_ROOT_DIR}
        ${_Globus_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

find_library(
    Globus_globus_gridmap_callout_error_LIBRARY
    NAMES globus_gridmap_callout_error
    PATHS 
        ${Globus_ROOT_DIR}
        ${_Globus_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

find_library(
    Globus_globus_gss_assist_LIBRARY
    NAMES globus_gss_assist
    PATHS 
        ${Globus_ROOT_DIR}
        ${_Globus_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

find_library(
    Globus_globus_gssapi_error_LIBRARY
    NAMES globus_gssapi_error
    PATHS 
        ${Globus_ROOT_DIR}
        ${_Globus_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

find_library(
    Globus_globus_gssapi_gsi_LIBRARY
    NAMES globus_gssapi_gsi
    PATHS 
        ${Globus_ROOT_DIR}
        ${_Globus_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

find_library(
    Globus_globus_xio_LIBRARY
    NAMES globus_xio
    PATHS 
        ${Globus_ROOT_DIR}
        ${_Globus_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

find_library(
    Globus_globus_xio_gsi_driver_LIBRARY
    NAMES globus_xio_gsi_driver
    PATHS 
        ${Globus_ROOT_DIR}
        ${_Globus_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Globus
    REQUIRED_VARS # The first one is displayed in the message
        Globus_globus_common_LIBRARY
        Globus_INCLUDE_DIRS
        Globus_globus_gridmap_callout_error_LIBRARY
        Globus_globus_gss_assist_LIBRARY
        Globus_globus_gssapi_error_LIBRARY
        Globus_globus_gssapi_gsi_LIBRARY
        Globus_globus_xio_gsi_driver_LIBRARY
        Globus_globus_xio_LIBRARY
)

if(Globus_FOUND)

    set(Globus_LIBRARIES
        ${Globus_globus_common_LIBRARY}
        ${Globus_globus_gridmap_callout_error_LIBRARY}
        ${Globus_globus_gss_assist_LIBRARY}
        ${Globus_globus_gssapi_error_LIBRARY}
        ${Globus_globus_gssapi_gsi_LIBRARY}
        ${Globus_globus_xio_gsi_driver_LIBRARY}
        ${Globus_globus_xio_LIBRARY}
    )

    if(NOT TARGET Globus::Globus)

        add_library(Globus::Globus INTERFACE IMPORTED)

        set_target_properties(
            Globus::Globus 
            PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${Globus_INCLUDE_DIRS}"
                INTERFACE_LINK_LIBRARIES "${Globus_LIBRARIES}"
        )
        
    endif()

endif()

mark_as_advanced(
    Globus_INCLUDE_DIRS
    Globus_LIBRARIES
    Globus_globus_common_LIBRARY
    Globus_globus_gridmap_callout_error_LIBRARY
    Globus_globus_gss_assist_LIBRARY
    Globus_globus_gssapi_error_LIBRARY
    Globus_globus_gssapi_gsi_LIBRARY
    Globus_globus_xio_gsi_driver_LIBRARY
    Globus_globus_xio_LIBRARY
)
