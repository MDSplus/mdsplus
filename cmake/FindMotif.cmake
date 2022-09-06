# FindMotif.cmake
#
# Finds Motif (Xm, Mrm, Uil) and X11 (X11, Xt)
#
# This will define the following variables
#
#   Motif_FOUND
#   Motif_INCLUDE_DIRS
#   Motif_Xm_INCLUDE_DIR
#   Motif_Mrm_INCLUDE_DIR
#   Motif_Uil_INCLUDE_DIR
#   Motif_X11_INCLUDE_DIR
#   Motif_Xt_INCLUDE_DIR
#   Motif_LIBRARIES
#   Motif_Xm_LIBRARY
#   Motif_Mrm_LIBRARY
#   Motif_Uil_LIBRARY
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

FIND_PATH(
    Motif_Xm_INCLUDE_DIR
    NAMES Xm/Xm.h
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        include
)

FIND_PATH(
    Motif_Mrm_INCLUDE_DIR
    NAMES Mrm/MrmPublic.h
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        include
)

FIND_PATH(
    Motif_Uil_INCLUDE_DIR
    NAMES uil/Uil.h
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        include
)

FIND_PATH(
    Motif_X11_INCLUDE_DIR
    NAMES X11/X.h
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        include
)

FIND_PATH(
    Motif_Xt_INCLUDE_DIR
    NAMES X11/Core.h
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        include
)

FIND_LIBRARY(
    Motif_Xm_LIBRARY
    NAMES Xm
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        lib
)

FIND_LIBRARY(
    Motif_Mrm_LIBRARY
    NAMES Mrm
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        lib
)

FIND_LIBRARY(
    Motif_Uil_LIBRARY
    NAMES Uil
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        lib
)

FIND_LIBRARY(
    Motif_X11_LIBRARY
    NAMES X11
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        lib
)

FIND_LIBRARY(
    Motif_Xt_LIBRARY
    NAMES Xt
    PATHS 
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES 
        lib
)

FIND_PROGRAM(
    uil_EXECUTABLE
    NAMES uil
    PATHS
        ${Motif_ROOT_DIR}
    PATH_SUFFIXES
        bin
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(
    Motif
    HANDLE_COMPONENTS
    REQUIRED_VARS 
        Motif_Xm_INCLUDE_DIR
        Motif_Mrm_INCLUDE_DIR
        Motif_Uil_INCLUDE_DIR
        Motif_X11_INCLUDE_DIR
        Motif_Xt_INCLUDE_DIR
        Motif_Xm_LIBRARY
        Motif_Mrm_LIBRARY
        Motif_Uil_LIBRARY
        Motif_X11_LIBRARY
        Motif_Xt_LIBRARY
        uil_EXECUTABLE
)

MARK_AS_ADVANCED(
    Motif_Xm_INCLUDE_DIR
    Motif_Mrm_INCLUDE_DIR
    Motif_Uil_INCLUDE_DIR
    Motif_X11_INCLUDE_DIR
    Motif_Xt_INCLUDE_DIR
    Motif_Xm_LIBRARY
    Motif_Mrm_LIBRARY
    Motif_Uil_LIBRARY
    Motif_X11_LIBRARY
    Motif_Xt_LIBRARY
    uil_EXECUTABLE
)

IF(Motif_FOUND)
    SET(Motif_INCLUDE_DIRS
        ${Motif_Xm_INCLUDE_DIR}
        ${Motif_Mrm_INCLUDE_DIR}
        ${Motif_Uil_INCLUDE_DIR}
        ${Motif_X11_INCLUDE_DIR}
        ${Motif_Xt_INCLUDE_DIR}
    )

    SET(Motif_LIBRARIES
        ${Motif_Xm_LIBRARY}
        ${Motif_Mrm_LIBRARY}
        ${Motif_Uil_LIBRARY}
        ${Motif_X11_LIBRARY}
        ${Motif_Xt_LIBRARY}
    )

    IF(NOT TARGET Motif::Motif)
        ADD_LIBRARY(Motif::Motif INTERFACE IMPORTED)
        SET_TARGET_PROPERTIES(
            Motif::Motif
            PROPERTIES
                INTERFACE_LINK_LIBRARIES "${Motif_LIBRARIES}"
                INTERFACE_INCLUDE_DIRECTORIES "${Motif_INCLUDE_DIRS}"
        )
    ENDIF()
    
ENDIF()
