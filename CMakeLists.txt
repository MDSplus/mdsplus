CMAKE_MINIMUM_REQUIRED(VERSION 3.20 FATAL_ERROR)

# Allow for custom CMake modules
LIST(PREPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

PROJECT(MDSplus)

ENABLE_LANGUAGE(Fortran)

ENABLE_TESTING()

###
### Options
###

# MESSAGE("${CMAKE_SYSTEM_NAME}")
# MESSAGE("${CMAKE_SYSTEM_PROCESSOR}")
# MESSAGE("${CMAKE_SYSTEM_PREFIX_PATH}")
# MESSAGE("${CMAKE_SYSTEM_INCLUDE_PATH}")
# MESSAGE("${CMAKE_SYSTEM_LIBRARY_PATH}")

# --x-includes=
# --x-libraries=
# --build= CMAKE_BUILD_TYPE?
# --host= CMAKE_SYSTEM_NAME?
# --target= CMAKE_SYSTEM_PROCESSOR?

# replaces --disable-shared
# BUILD_SHARED_LIBS

# replaces --disable-largefile
OPTION(ENABLE_LARGEFILE
    "Enable support for large files"
    ON
)

IF(NOT WIN32)
    # replaces --enable-perf
    OPTION(ENABLE_PERF
        "Collect performance statistics (linux only)"
        OFF
    )
ENDIF()

# replaces --disable-java
OPTION(ENABLE_JAVA
    "Build java libraries and applications"
    ON
)

# (SLW) TODO
# replaces --with-java_only
OPTION(JAVA_ONLY "")

# replaces --enable-d3d
OPTION(ENABLE_D3D
    "Build d3d ptdata access library located in $D3DLIB_PATH."
    OFF
)

# (SLW) TODO Investigate
# replaces --disable-xmltest
OPTION(ENABLE_XMLTEST
    "Try to compile and run a test LIBXML program."
    ON
)

# replaces --enable-valgrind
OPTION(ENABLE_VALGRIND
    "Enable Valgrind on the unit tests."
    ON
)

# TODO: doxygen dot, man, rtf, xml, chm, chi, html
# replaces --disable-doxygen-doc/dot/man/rtf/xml/chm/chi/html
OPTION(ENABLE_DOXYGEN
    "Generate doxygen documentation."
    ON
)

# replaces --enable-werror
OPTION(ENABLE_WERROR
    "Treat all warnings as errors (-Werror)."
    OFF
)

# replaces --disable-wreturns
OPTION(ENABLE_WRETURNS
    "Enable warnings about unused return values from functions (-Wreturns)."
    ON
)

# replaces --enable-wconversion
OPTION(ENABLE_WCONVERSION
    "Enable warnings about implicit conversions (-Wconversion)."
    OFF
)

# replaces --enable-sanitize=
SET(SANITIZER "address"
    CACHE STRING
    "Enable compile sanitizer with flavor, options are: address or thread. Default is address."
)
SET_PROPERTY(
    CACHE SANITIZER
    PROPERTY STRINGS "address" "thread" ""
)

# replaces --with-jars
SET(JARS ""
    CACHE STRING
    "???"
)

# replaces --with-jdk
SET(JDK_DIR ""
    CACHE STRING
    "Location of Java JDK"
)

# replaces --with-java_target=
SET(JAVA_TARGET_VERSION ""
    CACHE STRING
    "Version of compiled mdsobjects classes (i.e. 6)"
)

# replaces --with-java_bootclasspath=
SET(JAVA_BOOTCLASSPATH ""
    CACHE STRING
    "The bootclasspath option for use with JAVA_TARGET_VERSION (i.e. /usr/lib/jvm/java-1.6.0-openjdk-1.6.0.0/jre/lib/rt.jar)"
)

# replaces --with-gsi=globus-location:flavor
SET(GSI ""
    CACHE STRING
    "Set in the form of globus-location:flavor. Enables globus GSI for mdsip communications."
)

# replaces --with-srb=
SET(SRB_DIR ""
    CACHE STRING
    "Location of SRB to build against"
)

# replaces --with-labview=
SET(LABVIEW_DIR ""
    CACHE STRING
    "Location of labview (i.e. /usr/local/natinst/Labview)"
)

# replaces --with-idl=
OPTION(ENABLE_IDL
    "Build idlmdsevent and idlmdswidgets against the IDL installed in IDL_DIR. If IDL_DIR is not set, the system will be searched for an installation."
    ON
)

SET(IDL_DIR "" CACHE STRING
    "Location of IDL, or empty to indicate the system should be searched for an IDL installation. Setting this also sets ENABLE_IDL=ON."
)

IF(IDL_DIR AND NOT ENABLE_IDL)
    MESSAGE("Setting ENABLE_IDL=ON because IDL_DIR was specified")
    SET(ENABLE_IDL ON)
ENDIF()

# replaces --with-readline=
SET(READLINE_DIR ""
    CACHE STRING
    "Location of the readline library, if not set the system will be searched."
)

# replaces --with-xml-prefix, --with-xml-exec-prefix
SET(LIBXML2_DIR ""
    CACHE STRING
    "Location of libxml2 library"
)

# replaces --with-x
OPTION(
    ENABLE_X "" OFF
)

# replaces --with-valgrind-lib
SET(VALGRIND_LIB_DIR ""
    CACHE STRING
    ""
)

# replaces --with-winebottle
SET(WINEBOTTLE ""
    CACHE STRING
    ""
)

###
### Global Configuration
###

SET(CMAKE_C_FLAGS $ENV{CFLAGS})
SET(CMAKE_CXX_FLAGS $ENV{CXXFLAGS})
SET(CMAKE_Fortran_FLAGS $ENV{FCFLAGS})

# Generate compile_commands.json
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

ADD_DEFINITIONS(
    # (SLW) TODO: Move to testing/backends/check or remove altogether
    -DHAVE_CONFIG_H # Do we have mdsplus/mdsconfig.h
)

IF(MSVC)
    ADD_COMPILE_OPTIONS(/Wall)
ELSE()
    ADD_COMPILE_OPTIONS(-Wall -Wextra)
ENDIF()

IF(ENABLE_WERROR)
    IF(MSVC)
        ADD_COMPILE_OPTIONS(/WX)
        ELSE()
        ADD_COMPILE_OPTIONS(-Werror)
    ENDIF()
ENDIF()

IF(NOT ENABLE_WRETURNS) # -Wunused-result is on by default in gcc and clang
    IF(NOT MSVC)
        ADD_COMPILE_OPTIONS(-Wno-unused-result)
    ENDIF()
ENDIF()

IF(ENABLE_WCONVERSION)

ENDIF()

IF(ENABLE_LARGEFILE)
    IF(CMAKE_SIZEOF_VOID_P EQUAL 4)
        ADD_DEFINITIONS(
            -D_FILE_OFFSET_BITS=64
            -D_LARGE_FILES
        )
    ENDIF()
ENDIF()

###
### Dependencies
###

IF(ENABLE_JAVA)

    IF(JDK_DIR)
        SET(JAVA_HOME "${JDK_DIR}")
    ENDIF()

    FIND_PACKAGE(Java COMPONENTS Runtime Development REQUIRED)
    FIND_PACKAGE(JNI REQUIRED)
    
    INCLUDE(UseJava)

    FIND_PROGRAM(mvn_EXECUTABLE mvn)

ENDIF()

IF(ENABLE_IDL)

    IF(IDL_DIR)
        SET(IDL_ROOT_DIR "${IDL_DIR}")
    ELSEIF(NOT IDL_FOUND)
        SET(IDL_ROOT_DIR 
            /usr/local/harris/idl
            /usr/local/exelis/idl
            /usr/local/itt/idl
            /usr/local/rsi/idl
            ${CMAKE_SOURCE_DIR}/3rd-party-apis/idl
        )
    ENDIF()

    FIND_PACKAGE(IDL REQUIRED)

ENDIF()

IF(ENABLE_D3D)

    FIND_LIBRARY(
        LIBD3
        NAMES d3
        PATHS $ENV{D3DLIB_PATH}
        REQUIRED
    )

    MARK_AS_ADVANCED(LIBD3)

ENDIF()

IF(GSI)

    STRING(REPLACE ":" ";" _gsi_list "${GSI}")
    LIST(GET _gsi_list 0 Globus_ROOT_DIR)
    LIST(GET _gsi_list 1 Globus_FLAVOR)

    # (SLW) TODO: pthr???
        
    FIND_PACKAGE(Globus)

ENDIF()

FIND_PACKAGE(Python COMPONENTS Interpreter REQUIRED)

FIND_PACKAGE(libxml2 REQUIRED)

FIND_PACKAGE(Iconv REQUIRED)

SET(Readline_ROOT_DIR "${READLINE_DIR}")
FIND_PACKAGE(Readline REQUIRED)

FIND_PACKAGE(Motif)

FIND_PACKAGE(Sybase)

FIND_LIBRARY(LIBM m QUIET)
FIND_LIBRARY(LIBZ z QUIET)
FIND_LIBRARY(LIBLZMA lzma QUIET)
FIND_LIBRARY(LIBBLAS blas QUIET)
FIND_LIBRARY(LIBRT rt QUIET)
FIND_LIBRARY(LIBCURL curl QUIET)
SET(LIBDL "${CMAKE_DL_LIBS}")

FIND_PACKAGE(Threads REQUIRED)

MARK_AS_ADVANCED(LIBM LIBZ LIBLZMA LIBBLAS LIBDL)

###
### Check for Includes, Libraries, and Symbols
###

INCLUDE(CheckIncludeFile)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckSymbolExists)

CHECK_INCLUDE_FILE("alloca.h" HAVE_ALLOCA_H)
CHECK_INCLUDE_FILE("stdint.h" HAVE_STDINT_H)
CHECK_INCLUDE_FILE("grp.h" HAVE_GRP_H)
CHECK_INCLUDE_FILE("pwd.h" HAVE_PWD_H)
CHECK_SYMBOL_EXISTS(getpwuid "pwd.h" HAVE_GETPWUID)
CHECK_SYMBOL_EXISTS(clock_gettime "time.h" HAVE_CLOCK_GETTIME)
CHECK_SYMBOL_EXISTS(getaddrinfo "netdb.h" HAVE_GETADDRINFO)
CHECK_SYMBOL_EXISTS(mkstemp "stdlib.h" HAVE_MKSTEMP)

CHECK_INCLUDE_FILES("unistd.h" HAVE_UNISTD_H)
CHECK_SYMBOL_EXISTS(fork "unistd.h" HAVE_FORK)

CHECK_INCLUDE_FILES("sybdb.h;sybfront.h" HAVE_SYBASE) # TODO: Improve

CHECK_INCLUDE_FILE("vxWorks.h" HAVE_VXWORKS_H)

CHECK_INCLUDE_FILES("sys/fileio.h" HAVE_SYS_FILIO_H)
CHECK_INCLUDE_FILES("sys/types.h" HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE("sys/resource.h" HAVE_SYS_RESOURCE_H)
CHECK_SYMBOL_EXISTS(getrusage "sys/resource.h" HAVE_GETRUSAGE)

CHECK_INCLUDE_FILE("pthread.h" HAVE_PTHREAD_H)
CHECK_SYMBOL_EXISTS(pthread_lock_global_np "pthread.h" HAVE_PTHREAD_LOCK_GLOBAL_NP)

CHECK_INCLUDE_FILE("readline/readline.h" HAVE_READLINE_READLINE_H)
CHECK_SYMBOL_EXISTS("rl_set_signals" "readline/readline.h" HAVE_RL_SET_SIGNALS)

CHECK_INCLUDE_FILES("valgrind/valgrind.h" HAVE_VALGRIND_H)

CHECK_INCLUDE_FILES("scsi/sg.h" HAVE_SCSI_SG_H)

# TODO HAVE_VISIBILITY
SET(HAVE_VISIBILITY 1)

###
### Install
###

SET(BIN_DIR ${CMAKE_BINARY_DIR}/bin)
SET(LIB_DIR ${CMAKE_BINARY_DIR}/lib)
SET(ETC_DIR ${CMAKE_BINARY_DIR}/etc)
SET(UID_DIR ${CMAKE_BINARY_DIR}/uid)
SET(BIN_DIR ${CMAKE_BINARY_DIR}/bin)
SET(RPM_DIR ${CMAKE_BINARY_DIR}/rpm)

FILE(MAKE_DIRECTORY 
    ${BIN_DIR}
    ${LIB_DIR}
    ${ETC_DIR}
    ${UID_DIR}
    ${BIN_DIR}
    ${RPM_DIR}
)

SET(MODE_777
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_WRITE GROUP_EXECUTE
    WORLD_READ WORLD_WRITE WORLD_EXECUTE
)

SET(MODE_755
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ             GROUP_EXECUTE
    WORLD_READ             WORLD_EXECUTE
)

SET(MODE_644
    OWNER_READ OWNER_WRITE
    GROUP_READ            
    WORLD_READ            
)

INSTALL(
    DIRECTORY
        desktop
        idl
        include
        ${CMAKE_BINARY_DIR}/include
        epics
        LabView
        matlab
        nodejs
        php
        pixmaps
        pydevices
        tdi
        trees
        xml
        ${CMAKE_BINARY_DIR}/uid
    DESTINATION .
    USE_SOURCE_PERMISSIONS
    PATTERN "*.in" EXCLUDE
    PATTERN "Makefile.*" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
    PATTERN "*.pyc" EXCLUDE
    PATTERN "__pycache__" EXCLUDE
)

INSTALL(
    DIRECTORY python/MDSplus
    DESTINATION python
    USE_SOURCE_PERMISSIONS
    PATTERN "*.pyc" EXCLUDE
    PATTERN "__pycache__" EXCLUDE
    PATTERN "*.in" EXCLUDE
    PATTERN "Makefile.*" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
)

INSTALL(
    FILES MDSplus-License.rtf MDSplus-License.txt
    DESTINATION .
)

INSTALL(
    FILES setup.csh setup.sh
    DESTINATION .
    PERMISSIONS ${MODE_755}
)

INSTALL(
    DIRECTORY
    DESTINATION local/tdi
)

# (SLW) TODO: 
SET(LIBPATH "LD_LIBRARY_PATH")

###
### Generate _include/_mdsversion.h
###

INCLUDE(GitRevision)

# (SLW) TODO
SET(LIBPREFIX "Mds")

CONFIGURE_FILE(
    ${CMAKE_SOURCE_DIR}/_include/_mdsversion.h.in
    ${CMAKE_BINARY_DIR}/_include/_mdsversion.h
)

###
### Generate include/mdsplus/mdsconfig.h
###

SET(SHARELIB_TYPE "${CMAKE_SHARED_LIBRARY_SUFFIX}")

CONFIGURE_FILE(
    ${CMAKE_SOURCE_DIR}/include/mdsplus/mdsconfig.h.in
    ${CMAKE_BINARY_DIR}/include/mdsplus/mdsconfig.h
)

INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/_include
    ${CMAKE_SOURCE_DIR}/include/mdsplus

    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_BINARY_DIR}/_include
    ${CMAKE_BINARY_DIR}/include/mdsplus
)

###
### python/generate_tests
###

# FILE(GLOB
#     _test_case_list
#     "python/MDSplus/tests/*_case.py"
# )

# FOREACH(_test_case IN LISTS _test_case_list)
#     CMAKE_PATH(GET _test_case STEM _test_case_name)
#     ADD_TEST(
#         NAME ${_test_case_name}
#         COMMAND ${Python_EXECUTABLE} ${_test_case}
#     )

#     SET(_env_mods
#         "PATH=path_list_prepend:${BIN_DIR}"
#         "PYTHONPATH=set:${CMAKE_SOURCE_DIR}/python"
#         "PYTHONPATH=path_list_append:${CMAKE_BINARY_DIR}/python"
#     )

#     SET_TESTS_PROPERTIES(
#         ${_test_case_name}
#         PROPERTIES
#             ENVIRONMENT_MODIFICATION "${_env_mods}"
                
#     )
# ENDFOREACH()

###
### Generate include/*_messages.h
###

FILE(GLOB_RECURSE
    _messages_xml_list
    "*/*messages.xml"
)

SET(_messages_generated_list "python/MDSplus/mdsExceptions.py")

FOREACH(_messages_xml IN LISTS _messages_xml_list)
    CMAKE_PATH(GET _messages_xml STEM _stem)
    LIST(APPEND _messages_generated_list "include/${_stem}.h")
ENDFOREACH()

SET(_run_gen_messages FALSE)
FOREACH(_filename IN LISTS _messages_generated_list)
    IF(NOT EXISTS "${CMAKE_SOURCE_DIR}/${_filename}")
        MESSAGE("Missing ${_filename}, will run deploy/gen_messages.py")
        SET(_run_gen_messages TRUE)
    ENDIF()
ENDFOREACH()

IF(_run_gen_messages)
    MESSAGE("Running deploy/gen_messages.py")
    EXECUTE_PROCESS(
        COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/deploy/gen_messages.py
    )
ENDIF()

# MESSAGE("${_messages_generated_list}")

# ADD_CUSTOM_COMMAND(
#     COMMENT "Running deploy/gen_messages.py"
#     OUTPUT ${_messages_generated_list}
#     DEPENDS ${_messages_xml_list}
#     COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/deploy/gen_messages.py
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
# )

###
### Generate python/MDSplus/compound.py
###

IF(NOT EXISTS "${CMAKE_SOURCE_DIR}/python/MDSplus/compound.py")
    MESSAGE("Missing python/MDSplus/compound.py, will run deploy/gen_compound_opcbuiltins.py")
    EXECUTE_PROCESS(
        COMMAND ${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/deploy/gen_compound_opcbuiltins.py
    )
ENDIF()

###
### Documentation
###

FIND_PACKAGE(Doxygen)

SET(DOXYGEN_PROGRAM $<TARGET_FILE:Doxygen::doxygen>)

ADD_CUSTOM_TARGET(
    docs
)

###
### Libraries, Executables
###

INCLUDE(AddStaticCopy)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR})

# Fortran Libraries

ADD_SUBDIRECTORY(math)
ADD_SUBDIRECTORY(d3dshr)

# Core Libraries

ADD_SUBDIRECTORY(mdsshr)
ADD_SUBDIRECTORY(treeshr) # Requires MdsShr
ADD_SUBDIRECTORY(xtreeshr) # Requires TreeShr
ADD_SUBDIRECTORY(tdishr) # Requires TreeShr
ADD_SUBDIRECTORY(mdstcpip) # Requires TdiShr
ADD_SUBDIRECTORY(mdslib) # Requires TdiShr
ADD_SUBDIRECTORY(xmdsshr) # Requires TdiShr
ADD_SUBDIRECTORY(mdsmisc) # Requires XTreeShr
ADD_SUBDIRECTORY(mdsdcl) # Requires MdsIpShr
ADD_SUBDIRECTORY(servershr) # Requires Mdsdcl
ADD_SUBDIRECTORY(mdssql)

# Language Binding Libraries

ADD_SUBDIRECTORY(mdslibidl) # Requires TdiShr
ADD_SUBDIRECTORY(javamds) # Requires TdiShr
ADD_SUBDIRECTORY(mdsobjects/cpp) # Requires JavaMds
ADD_SUBDIRECTORY(mdsobjects/labview) # Requires MdsObjectsCppShr
ADD_SUBDIRECTORY(python)
ADD_SUBDIRECTORY(java)

# Command Line Tools

ADD_SUBDIRECTORY(tdic)
ADD_SUBDIRECTORY(tditest)
ADD_SUBDIRECTORY(setevent)
ADD_SUBDIRECTORY(wfevent)
# actions has actlog

# Graphical Tools

ADD_SUBDIRECTORY(actions)
ADD_SUBDIRECTORY(dwscope)
ADD_SUBDIRECTORY(traverser)
ADD_SUBDIRECTORY(idlmdsevent)
ADD_SUBDIRECTORY(idlmdswidgets)

# Device Support

add_subdirectory(mitdevices)
ADD_SUBDIRECTORY(camshr)
ADD_SUBDIRECTORY(remcam) # Requires camshr
ADD_SUBDIRECTORY(pydevices)

# Mdsdcl commands

ADD_SUBDIRECTORY(tcl)
ADD_SUBDIRECTORY(ccl) # Requires CamShr
# camshr has cts_commands

# Globus

ADD_SUBDIRECTORY(roam)
# mdstcpip has MdsIpGSI

# Configuration

ADD_SUBDIRECTORY(rpm)
ADD_SUBDIRECTORY(scripts)
ADD_SUBDIRECTORY(manpages)

###
### Testing
###

ADD_SUBDIRECTORY(testing)
ADD_SUBDIRECTORY(mdsshr/testing)
# ADD_SUBDIRECTORY(treeshr/testing)
# ADD_SUBDIRECTORY(mdslib/testing)
# ADD_SUBDIRECTORY(mdstcpip/testing)
ADD_SUBDIRECTORY(tdishr/testing)
# ADD_SUBDIRECTORY(tditest/testing)
# ADD_SUBDIRECTORY(mdsobjects/cpp/testing)
ADD_SUBDIRECTORY(python/MDSplus/tests)

IF(Motif_FOUND)

    ###
    ### Process *.uil
    ###

    FILE(GLOB_RECURSE
        _uil_list
        "actions/*.uil"
        "dwscope/*.uil"
        "traverser/*.uil"
        "mitdevices/*.uil"
        "idlmdswidgets/*.uil"
        "xmdsshr/*.uil"
    )

    FOREACH(_uil IN LISTS _uil_list)
        CMAKE_PATH(GET _uil STEM LAST_ONLY _stem)
        SET(_uid ${UID_DIR}/${_stem}.uid)
        
        ADD_CUSTOM_COMMAND(
            OUTPUT ${_uid}
            DEPENDS ${_uil}
            COMMAND ${uil_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/include -o ${_uid} ${_uil}
        )

        LIST(APPEND _uid_list ${_uid})
    ENDFOREACH()

    ADD_CUSTOM_TARGET(
        generate-uids ALL
        DEPENDS ${_uid_list}
    )

ENDIF()

###
### ChangeLog
###

GIT(GIT_CHANGELOG log --decorate=full --no-merges)

FILE(WRITE ${CMAKE_BINARY_DIR}/ChangeLog "${GIT_CHANGELOG}")

INSTALL(
    FILES ${CMAKE_BINARY_DIR}/ChangeLog
    DESTINATION .
)