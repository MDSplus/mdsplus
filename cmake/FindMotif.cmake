# FindMotif.cmake
#
# Finds Motif (Xm, Mrm, uil) and X11 (X11, Xt)
#
# This will define the following variables
#
#   Motif_FOUND
#   Motif_INCLUDE_DIRS
#   Motif_Xm_INCLUDE_DIR
#   Motif_Mrm_INCLUDE_DIR
#   Motif_X11_INCLUDE_DIR
#   Motif_Xt_INCLUDE_DIR
#   Motif_LIBRARIES
#   Motif_Xm_LIBRARY
#   Motif_Mrm_LIBRARY
#   Motif_X11_LIBRARY
#   Motif_Xt_LIBRARY
#   uil_EXECUTABLE
#
# and the following imported targets
#
#   Motif::Motif
#
# The following variables can be set as arguments
#
#   Motif_ROOT_DIR
#

find_path(
    Motif_Xm_INCLUDE_DIR
    NAMES Xm/Xm.h
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        include
)

find_path(
    Motif_Mrm_INCLUDE_DIR
    NAMES Mrm/MrmPublic.h
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        include
)

find_path(
    Motif_X11_INCLUDE_DIR
    NAMES X11/X.h
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        include
)

find_path(
    Motif_Xt_INCLUDE_DIR
    NAMES X11/Core.h
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        include
)

find_library(
    Motif_Xm_LIBRARY
    NAMES Xm
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        lib
)

find_library(
    Motif_Mrm_LIBRARY
    NAMES Mrm
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        lib
)

find_library(
    Motif_X11_LIBRARY
    NAMES X11
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        lib
)

find_library(
    Motif_Xt_LIBRARY
    NAMES Xt
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        lib
)

find_program(
    uil_EXECUTABLE
    NAMES uil
    PATHS
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES
        bin
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Motif
    REQUIRED_VARS # The first one is displayed in the message
        Motif_Xm_LIBRARY
        Motif_Xm_INCLUDE_DIR
        Motif_Mrm_INCLUDE_DIR
        Motif_X11_INCLUDE_DIR
        Motif_Xt_INCLUDE_DIR
        Motif_Mrm_LIBRARY
        Motif_X11_LIBRARY
        Motif_Xt_LIBRARY
        uil_EXECUTABLE
)

if(Motif_FOUND)

    set(Motif_INCLUDE_DIRS
        ${Motif_Xm_INCLUDE_DIR}
        ${Motif_Mrm_INCLUDE_DIR}
        ${Motif_X11_INCLUDE_DIR}
        ${Motif_Xt_INCLUDE_DIR}
    )

    set(Motif_LIBRARIES
        ${Motif_Xm_LIBRARY}
        ${Motif_Mrm_LIBRARY}
        ${Motif_X11_LIBRARY}
        ${Motif_Xt_LIBRARY}
    )

    if(NOT TARGET Motif::Motif)

        add_library(Motif::Motif INTERFACE IMPORTED)

        set_target_properties(
            Motif::Motif
            PROPERTIES
                INTERFACE_LINK_LIBRARIES "${Motif_LIBRARIES}"
                INTERFACE_INCLUDE_DIRECTORIES "${Motif_INCLUDE_DIRS}"
        )
        
    endif()
    
endif()

mark_as_advanced(
    Motif_Xm_INCLUDE_DIR
    Motif_Mrm_INCLUDE_DIR
    Motif_X11_INCLUDE_DIR
    Motif_Xt_INCLUDE_DIR
    Motif_Xm_LIBRARY
    Motif_Mrm_LIBRARY
    Motif_X11_LIBRARY
    Motif_Xt_LIBRARY
    uil_EXECUTABLE
)