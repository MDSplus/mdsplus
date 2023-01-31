# FindGlobus.cmake
#
# Finds the Globus Toolkit (or Grid Community Toolkit)
#
# This will define the following variables
#
#   Globus_FOUND
#   Globus_INCLUDE_DIR
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

pkg_check_modules(_globus_common_PC                 QUIET globus-common)
pkg_check_modules(_globus_gridmap_callout_error_PC  QUIET globus-gridmap-callout-error)
pkg_check_modules(_globus_gss_assist_PC             QUIET globus-gss-assist)
pkg_check_modules(_globus_gssapi_error_PC           QUIET globus-gssapi-error)
pkg_check_modules(_globus_gssapi_gsi_PC             QUIET globus-gssapi-gsi)
pkg_check_modules(_globus_xio_gsi_driver_PC         QUIET globus-xio-gsi-driver)
pkg_check_modules(_globus_xio_PC                    QUIET globus-xio)

find_path(
    Globus_INCLUDE_DIR
    NAMES globus_common.h
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_common_PC_INCLUDE_DIRS}
    PATH_SUFFIXES
        include
)

find_library(
    Globus_globus_common_LIBRARY
    NAMES globus_common
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_common_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

find_library(
    Globus_globus_gridmap_callout_error_LIBRARY
    NAMES globus_gridmap_callout_error
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_gridmap_callout_error_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

find_library(
    Globus_globus_gss_assist_LIBRARY
    NAMES globus_gss_assist
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_gss_assist_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

find_library(
    Globus_globus_gssapi_error_LIBRARY
    NAMES globus_gssapi_error
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_gssapi_error_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

find_library(
    Globus_globus_gssapi_gsi_LIBRARY
    NAMES globus_gssapi_gsi
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_gssapi_gsi_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

find_library(
    Globus_globus_xio_LIBRARY
    NAMES globus_xio
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_xio_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

find_library(
    Globus_globus_xio_gsi_driver_LIBRARY
    NAMES globus_xio_gsi_driver
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_xio_gsi_driver_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Globus
    HANDLE_COMPONENTS
    REQUIRED_VARS 
        Globus_INCLUDE_DIR
        Globus_globus_common_LIBRARY
        Globus_globus_gridmap_callout_error_LIBRARY
        Globus_globus_gss_assist_LIBRARY
        Globus_globus_gssapi_error_LIBRARY
        Globus_globus_gssapi_gsi_LIBRARY
        Globus_globus_xio_gsi_driver_LIBRARY
        Globus_globus_xio_LIBRARY
)

mark_as_advanced(
    Globus_INCLUDE_DIR
    Globus_globus_common_LIBRARY
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
                INTERFACE_INCLUDE_DIRECTORIES "${Globus_INCLUDE_DIR}"
                INTERFACE_LINK_LIBRARIES "${Globus_LIBRARIES}"
        )
    endif()
endif()