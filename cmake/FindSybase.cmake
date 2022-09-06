# FindSybase.cmake
#
# Finds Sybase (or mssql, or freetds)
#
# This will define the following variables
#
#   Sybase_FOUND
#   Sybase_INCLUDE_DIRS
#   Sybase_LIBRARIES
#   Sybase_COMPILE_DEFINITIONS
#
# and the following imported targets
#
#   Sybase::Sybase
#
# The following variables can be set as arguments
#
#   Sybase_ROOT_DIR
#

FIND_PACKAGE(PkgConfig QUIET)

# PKG_CHECK_MODULES(_libxml2_PC QUIET libxml2)
# IF(NOT _libxml2_PC_FOUND)
#     PKG_CHECK_MODULES(_libxml2_PC QUIET libxml-2.0)
# ENDIF()

IF(WIN32)

    # SET(Sybase_CFLAGS -DSYBASE)
    # SET(Sybase_INCLUDE_DIRS "")
    # SET(Sybase_LIBRARIES ntwdblib${arch})

ELSE()

    SET(Sybase_COMPILE_DEFINITIONS SYBASE)

    FIND_PATH(
        Sybase_INCLUDE_DIRS
        NAMES sybdb.h
        PATHS 
            ${Sybase_ROOT_DIR}
        PATH_SUFFIXES 
            include
    )

    FIND_LIBRARY(
        Sybase_LIBRARIES
        NAMES sybdb
        PATHS 
            ${Sybase_ROOT_DIR}
        PATH_SUFFIXES 
            lib
    )

ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    Sybase
    HANDLE_COMPONENTS
    REQUIRED_VARS 
        Sybase_INCLUDE_DIRS
        Sybase_LIBRARIES
)

MARK_AS_ADVANCED(
    Sybase_CFLAGS
    Sybase_INCLUDE_DIRS
    Sybase_LIBRARIES
)

IF(Sybase_FOUND)
    IF(NOT TARGET Sybase::Sybase)
        ADD_LIBRARY(Sybase::Sybase INTERFACE IMPORTED)
        SET_TARGET_PROPERTIES(
            Sybase::Sybase
            PROPERTIES
                INTERFACE_COMPILE_DEFINITIONS "${Sybase_COMPILE_DEFINITIONS}"
                INTERFACE_INCLUDE_DIRECTORIES "${Sybase_INCLUDE_DIRS}"
                INTERFACE_LINK_LIBRARIES "${Sybase_LIBRARIES}"
        )
    ENDIF()
ENDIF()