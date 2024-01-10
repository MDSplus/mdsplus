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

find_package(PkgConfig QUIET)

if(WIN32)

    # SET(Sybase_CFLAGS -DSYBASE)
    # SET(Sybase_INCLUDE_DIRS "")
    # SET(Sybase_LIBRARIES ntwdblib${arch})

else()

    set(Sybase_COMPILE_DEFINITIONS SYBASE)

    find_path(
        Sybase_INCLUDE_DIRS
        NAMES sybdb.h
        PATHS 
            ${Sybase_ROOT_DIR}
        PATH_SUFFIXES 
            include
            include/freetds
    )

    find_library(
        Sybase_LIBRARIES
        NAMES sybdb
        PATHS 
            ${Sybase_ROOT_DIR}
        PATH_SUFFIXES 
            lib
    )

endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Sybase
    REQUIRED_VARS 
        Sybase_LIBRARIES
        Sybase_INCLUDE_DIRS
)

mark_as_advanced(
    Sybase_CFLAGS
    Sybase_INCLUDE_DIRS
    Sybase_LIBRARIES
)

if(Sybase_FOUND)

    if(NOT TARGET Sybase::Sybase)

        add_library(Sybase::Sybase INTERFACE IMPORTED)
        
        set_target_properties(
            Sybase::Sybase
            PROPERTIES
                INTERFACE_COMPILE_DEFINITIONS "${Sybase_COMPILE_DEFINITIONS}"
                INTERFACE_INCLUDE_DIRECTORIES "${Sybase_INCLUDE_DIRS}"
                INTERFACE_LINK_LIBRARIES "${Sybase_LIBRARIES}"
        )

    endif()

endif()