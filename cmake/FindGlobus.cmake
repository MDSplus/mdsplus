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

FIND_PACKAGE(PkgConfig QUIET)

PKG_CHECK_MODULES(_globus_common_PC                 QUIET globus-common)
PKG_CHECK_MODULES(_globus_gridmap_callout_error_PC  QUIET globus-gridmap-callout-error)
PKG_CHECK_MODULES(_globus_gss_assist_PC             QUIET globus-gss-assist)
PKG_CHECK_MODULES(_globus_gssapi_error_PC           QUIET globus-gssapi-error)
PKG_CHECK_MODULES(_globus_gssapi_gsi_PC             QUIET globus-gssapi-gsi)
PKG_CHECK_MODULES(_globus_xio_gsi_driver_PC         QUIET globus-xio-gsi-driver)
PKG_CHECK_MODULES(_globus_xio_PC                    QUIET globus-xio)

FIND_PATH(
    Globus_INCLUDE_DIR
    NAMES globus_common.h
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_common_PC_INCLUDE_DIRS}
    PATH_SUFFIXES
        include
)

FIND_LIBRARY(
    Globus_globus_common_LIBRARY
    NAMES globus_common
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_common_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

FIND_LIBRARY(
    Globus_globus_gridmap_callout_error_LIBRARY
    NAMES globus_gridmap_callout_error
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_gridmap_callout_error_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

FIND_LIBRARY(
    Globus_globus_gss_assist_LIBRARY
    NAMES globus_gss_assist
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_gss_assist_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

FIND_LIBRARY(
    Globus_globus_gssapi_error_LIBRARY
    NAMES globus_gssapi_error
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_gssapi_error_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

FIND_LIBRARY(
    Globus_globus_gssapi_gsi_LIBRARY
    NAMES globus_gssapi_gsi
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_gssapi_gsi_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

FIND_LIBRARY(
    Globus_globus_xio_LIBRARY
    NAMES globus_xio
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_xio_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

FIND_LIBRARY(
    Globus_globus_xio_gsi_driver_LIBRARY
    NAMES globus_xio_gsi_driver
    PATHS 
        ${Globus_ROOT_DIR}
        ${_globus_xio_gsi_driver_PC_LIBRARY_DIRS}
    PATH_SUFFIXES 
        lib
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
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

MARK_AS_ADVANCED(
    Globus_INCLUDE_DIR
    Globus_globus_common_LIBRARY
    Globus_globus_gridmap_callout_error_LIBRARY
    Globus_globus_gss_assist_LIBRARY
    Globus_globus_gssapi_error_LIBRARY
    Globus_globus_gssapi_gsi_LIBRARY
    Globus_globus_xio_gsi_driver_LIBRARY
    Globus_globus_xio_LIBRARY
)

IF(Globus_FOUND)
    SET(Globus_LIBRARIES
        ${Globus_globus_common_LIBRARY}
        ${Globus_globus_gridmap_callout_error_LIBRARY}
        ${Globus_globus_gss_assist_LIBRARY}
        ${Globus_globus_gssapi_error_LIBRARY}
        ${Globus_globus_gssapi_gsi_LIBRARY}
        ${Globus_globus_xio_gsi_driver_LIBRARY}
        ${Globus_globus_xio_LIBRARY}
    )

    IF(NOT TARGET Globus::Globus)
        ADD_LIBRARY(Globus::Globus INTERFACE IMPORTED)
        SET_TARGET_PROPERTIES(
            Globus::Globus 
            PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${Globus_INCLUDE_DIR}"
                INTERFACE_LINK_LIBRARIES "${Globus_LIBRARIES}"
        )
    ENDIF()
ENDIF()