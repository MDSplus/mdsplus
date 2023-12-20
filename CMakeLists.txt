cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-Source builds are no longer supported. Make a build directory, and run cmake from there instead.")
endif()

# Allow for custom CMake modules
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Disable "Built target" messages for Makefiles
set(CMAKE_TARGET_MESSAGES OFF CACHE BOOL "")
mark_as_advanced(CMAKE_TARGET_MESSAGES)

project(MDSplus)

enable_language(Fortran)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 98)

# Query version and release information from git
include(MDSplusGitRevision)

include(MDSplusOption)

###
### Download 3rd-party-apis
###

mdsplus_option(
    DOWNLOAD_3RD_PARTY_APIS BOOL
    "Download https://github.com/MDSplus/3rd-party-apis to help build IDL and LabView, and to help building on Windows."
    DEFAULT ON
)

if(DOWNLOAD_3RD_PARTY_APIS)

    set(_3rd_party_apis ${CMAKE_BINARY_DIR}/3rd-party-apis-master)

    if(NOT EXISTS ${_3rd_party_apis})

        set(_3rd_party_apis_url "https://github.com/MDSplus/3rd-party-apis/archive/refs/heads/master.zip")
        set(_3rd_party_apis_zip ${CMAKE_BINARY_DIR}/3rd-party-apis-master.zip)

        message(STATUS "Downloading and extracting ${_3rd_party_apis_url}")

        file(DOWNLOAD ${_3rd_party_apis_url} ${_3rd_party_apis_zip})

        # We sadly have no control over the name of the folder it gets extracted to
        # For GitHub, it will always be "REPOSITORY_NAME-BRANCH"
        file(ARCHIVE_EXTRACT 
            INPUT ${_3rd_party_apis_zip}
            DESTINATION ${CMAKE_BINARY_DIR}
        )

    endif()
    
endif()

###
### Options
###

# --x-includes=
# --x-libraries=
# --build=
# --host= CMAKE_SYSTEM_NAME?
# --target= CMAKE_SYSTEM_PROCESSOR?

# replaces --disable-shared
mdsplus_option(
    BUILD_SHARED_LIBS BOOL
    "Build shared libraries."
    DEFAULT ON
)

# If no configuration types are defined, use the default list
if(NOT DEFINED CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES
        "Debug"
        "Release"
        "MinSizeRel"
        "RelWithDebInfo"
    )
endif()

# We have to be careful not to overwrite the current CMAKE_BUILD_TYPE, if there is one
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(_force_build_type FORCE)
endif()

# replaces --enable-debug
mdsplus_option(
    CMAKE_BUILD_TYPE STRING
    "Type of build to configure."
    OPTIONS ${CMAKE_CONFIGURATION_TYPES}
    DEFAULT "Release"
    ${_force_build_type}
)

# TODO: APPLE?
if(WIN32)
    set(_default_install_dir "C:\\Program Files\\MDSplus")
else()
    set(_default_install_dir "/usr/local/mdsplus")
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(_override_install_dir FORCE)
endif()

# replaces --prefix
mdsplus_option(
    CMAKE_INSTALL_PREFIX STRING
    "Where to install files to."
    DEFAULT "${_default_install_dir}"
    ${_override_install_dir}
)

# Must be included after CMAKE_INSTALL_PREFIX
# Defines the following that we use:
# * CMAKE_INSTALL_BINDIR
# * CMAKE_INSTALL_LIBDIR
# * CMAKE_INSTALL_SYSCONFIGDIR
# * CMAKE_INSTALL_INCLUDEDIR
include(GNUInstallDirs)

# Defines option(BUILD_TESTING)
include(CTest)

if(BUILD_TESTING)

    mdsplus_option(
        ENABLE_SELFTEST BOOL
        "Include the testing/selftest directory."
        DEFAULT OFF
    )
    
    # replaces --enable-valgrind
    mdsplus_option(
        ENABLE_VALGRIND BOOL
        "Run all of the tests through Valgrind."
        DEFAULT OFF
    )

    set(_valgrind_tool_list
        memcheck    # https://valgrind.org/docs/manual/mc-manual.html
        helgrind    # https://valgrind.org/docs/manual/hg-manual.html
        drd         # https://valgrind.org/docs/manual/drd-manual.html
        exp-sgcheck # https://valgrind.org/docs/manual/sg-manual.html
    )

    mdsplus_option(
        VALGRIND_TOOLS STRING
        "The list of valgrind tools to search for."
        DEFAULT "${_valgrind_tool_list}"
    )

    # replaces --with-valgrind-lib
    mdsplus_option(
        VALGRIND_LIB_DIR STRING
        "Location of the valgrind library"
    )

    set(_sanitize_flavor_list
        memory      # (msan)  https://clang.llvm.org/docs/MemorySanitizer.html
        address     # (asan)  https://clang.llvm.org/docs/AddressSanitizer.html
        thread      # (tsan)  https://clang.llvm.org/docs/ThreadSanitizer.html
        undefined   # (ubsan) https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
    )

    mdsplus_option(
        ENABLE_SANITIZE STRING
        "Enable compile sanitizer with flavor."
        OPTIONS 
            OFF
            ${_sanitize_flavor_list}
        DEFAULT OFF
    )
    
    mdsplus_option(
        GENERATE_VSCODE_LAUNCH_JSON BOOL
        "Generate .vscode/launch.json, this option disables itself after it is run."
        DEFAULT OFF
    )

endif()

mdsplus_option(
    PYTHON STRING
    "Path to python interpreter to use for building and testing."
    DEFAULT "$PYTHON"
)

if(PYTHON STREQUAL "$PYTHON")
    set(PYTHON "$ENV{PYTHON}")
endif()

mdsplus_option(
    ENABLE_MOTIF BOOL
    "Build motif libraries and applications."
    DEFAULT ON
)

mdsplus_option(
    ENABLE_SYBASE BOOL
    "Build mdssql library, and Sybase support in TDI."
    DEFAULT OFF
)

mdsplus_option(
    ENABLE_HDF5 BOOL
    "Build HDF5 libraries and applications."
    DEFAULT OFF
)

# replaces --enable-d3d
mdsplus_option(
    ENABLE_D3D BOOL
    "Build MdsD3D library against d3 library located in $D3DLIB_PATH."
    DEFAULT OFF
)

# replaces --disable-java
mdsplus_option(
    ENABLE_JAVA BOOL
    "Build java libraries and applications."
    DEFAULT ON
)

# (SLW) TODO
# replaces --with-java_only
#option(JAVA_ONLY "")

# replaces --with-jars
# set(JARS ""
#     CACHE STRING
#     "???"
# )

# replaces --with-jdk
mdsplus_option(
    JAVA_HOME STRING
    "Location of Java Development Kit (JDK)."
    DEFAULT "$JAVA_HOME"
)

if(JAVA_HOME STREQUAL "$JAVA_HOME")
    set(JAVA_HOME "$ENV{JAVA_HOME}")
endif()

if(APPLE)
    set(_java_version_description ", if JAVA_HOME is not set, this will be passed to `/usr/libexec/java_home -v $JAVA_VERSION` to find a specific version")
endif()

mdsplus_option(
    JAVA_VERSION STRING
    "Minimum version of Java to search for${_java_version_description}."
)

if(WIN32)
    # replaces --disable-largefile
    mdsplus_option(
        ENABLE_LARGEFILE BOOL
        "Enable support for large files when compiling for 32-bit."
        DEFAULT ON
    )
endif()

if(NOT WIN32)
    # replaces --enable-perf
    mdsplus_option(
        ENABLE_TREE_PERF BOOL
        "Collect Tree I/O performance statistics and store them in $mds_perf_filename."
        DEFAULT OFF
    )
endif()

# replaces --enable-werror
mdsplus_option(
    ENABLE_WERROR BOOL
    "Treat all warnings as errors (-Werror)."
    DEFAULT OFF
)

# replaces --enable-debug=profile
mdsplus_option(
    ENABLE_GPROF BOOL
    "Enable profiling with gprof (compile and link with -pg)."
    DEFAULT OFF
)

# replaces --with-gsi=globus-location:flavor
mdsplus_option(
    GSI STRING
    "Set in the form of flavor or globus-location:flavor. Enables globus GSI for mdsip communications."
)

# TODO: Investigate
# replaces --with-srb=
mdsplus_option(
    SRB_DIR STRING
    "Location of SRB to build against."
)

mdsplus_option(
    ENABLE_LABVIEW BOOL
    "Build LV and MDSobjectsLVShr and install MDSplus LabView projects."
    DEFAULT ON
)

# replaces --with-labview=
mdsplus_option(
    LABVIEW_DIR STRING
    "Location of labview (i.e. /usr/local/natinst/Labview). Setting this also sets ENABLE_LABVIEW=ON."
)

if(LABVIEW_DIR AND NOT ENABLE_LABVIEW)

    message(STATUS "Setting ENABLE_LABVIEW=ON because LABVIEW_DIR was specified")

    get_property(_helpstring CACHE ENABLE_LABVIEW PROPERTY HELPSTRING)
    set(ENABLE_LABVIEW ON CACHE STRING "${_helpstring}" FORCE)

endif()

# replaces --with-idl=
mdsplus_option(
    ENABLE_IDL BOOL
    "Build idlmdsevent and idlmdswidgets against the IDL installed in IDL_DIR. If IDL_DIR is not set, the system will be searched for an installation."
    DEFAULT ON
)

mdsplus_option(
    IDL_DIR STRING
    "Location of IDL, or empty to indicate the system should be searched for an IDL installation. Setting this also sets ENABLE_IDL=ON."
)

if(IDL_DIR AND NOT ENABLE_IDL)

    message(STATUS "Setting ENABLE_IDL=ON because IDL_DIR was specified")

    get_property(_helpstring CACHE ENABLE_IDL PROPERTY HELPSTRING)
    set(ENABLE_IDL ON CACHE STRING "${_helpstring}" FORCE)

endif()

# replaces --with-readline=
mdsplus_option(
    READLINE_DIR STRING
    "Location of the readline library, if not set the system will be searched"
)

# replaces --with-xml-prefix, --with-xml-exec-prefix
mdsplus_option(
    LIBXML2_DIR STRING
    "Location of libxml2 library"
)

mdsplus_option(
    NO_SYSTEM_BLAS BOOL
    "Use the local copy of blas in math/blas/ instead of a system version"
    DEFAULT OFF
)

# TODO: doxygen dot, man, rtf, xml, chm, chi, html
# replaces --enable/disable-doxygen-doc/dot/man/rtf/xml/chm/chi/html
mdsplus_option(
    ENABLE_DOXYGEN BOOL
    "Generate doxygen documentation."
    DEFAULT ON
)

# TODO:
# replaces --with-x
# option(ENABLE_X
#     "" 
#     OFF
# )


# TODO:
# replaces --with-winebottle
# set(WINEBOTTLE ""
#     CACHE STRING
#     ""
# )

###
### Global Configuration
###

# $ENV{CFLAGS} is added by default
# $ENV{CXXFLAGS} is added by default
# $ENV{FFLAGS} is added by default
set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} $ENV{FCFLAGS}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

include_directories(
    ${CMAKE_SOURCE_DIR}/_include
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/mdsplus
    # For mdsplus/mdsconfig.h
    ${CMAKE_BINARY_DIR}/include
    ${CMAKE_BINARY_DIR}/include/mdsplus
)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
mark_as_advanced(CMAKE_EXPORT_COMPILE_COMMANDS)

add_compile_definitions(
    # (SLW) TODO: Move to testing/backends/check or remove altogether
    HAVE_CONFIG_H # Do we have mdsplus/mdsconfig.h
)

include(MDSplusCheckFlags)
include(MDSplusAddCompilerFlag)

if(ENABLE_TESTS)
    include(CTest)
endif()

if(ENABLE_TREE_PERF)
    add_compile_definitions(USE_TREE_PERF)
endif()

if(MSVC)
    add_compile_options(/Wall)
else()

    # TODO: Disable for flang (family: LLVMFlang)

    mdsplus_add_compiler_flags(-Wall)
    mdsplus_add_compiler_flags(-Wextra)

    if(CMAKE_Fortran_COMPILER_ID STREQUAL "Intel")
        set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -warn all")
    endif()

endif()
    
if(ENABLE_WERROR)

    if(MSVC)
        add_compile_options(/WX)
    else()

        mdsplus_add_compiler_flags(-Werror)

        if(CMAKE_Fortran_COMPILER_ID STREQUAL "Intel")
            set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -warn error")
        endif()

    endif()

endif()

if(ENABLE_GPROF)

    string(TOLOWER "${CMAKE_BUILD_TYPE}" _build_type)
    if(NOT _build_type STREQUAL "relwithdebinfo")
        message(WARNING "It is recommended to use CMAKE_BUILD_TYPE=RelWithDebInfo with ENABLE_GPROF=ON")    
    endif()

    if(NOT CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
        NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
        NOT CMAKE_Fortran_COMPILER_ID STREQUAL "GNU")

        message(WARNING "Unable to build with support for gprof, -pg is only supported on GNU compilers")

    else()

        add_compile_options(-pg)
        add_link_options(-pg)
        
    endif()

endif()

if(ENABLE_LARGEFILE)

    # If we are compiling for 32-bit
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        add_compile_definitions(
            _FILE_OFFSET_BITS=64
            _LARGE_FILES
        )
    endif()

endif()

if(ENABLE_SANITIZE IN_LIST _sanitize_flavor_list)

    set(_prefix "_sanitize_${ENABLE_SANITIZE}")

    mdsplus_check_flags(${_prefix}
        COMPILER -fsanitize=${ENABLE_SANITIZE} -O3
        LINKER   -fsanitize=${ENABLE_SANITIZE}
    )

    foreach(_lang IN ITEMS C CXX)
        if(${_prefix}_${_lang})
            set(CMAKE_${_lang}_FLAGS "${CMAKE_${_lang}_FLAGS} -fsanitize=${ENABLE_SANITIZE} -O3")
        endif()
    endforeach()

    # TODO: Improve
    add_link_options(-fsanitize=${ENABLE_SANITIZE})

endif()

foreach(_lang IN ITEMS C CXX)
    if((CMAKE_${_lang}_COMPILER_ID STREQUAL "Clang" AND CMAKE_${_lang}_COMPILER_VERSION GREATER_EQUAL "15") 
        OR (CMAKE_${_lang}_COMPILER_ID STREQUAL "AppleClang" AND CMAKE_${_lang}_COMPILER_VERSION GREATER_EQUAL "14"))

        # Disables the new strict prototype checking that is on by default in clang >= 15 (or apple clang >= 14)
        set(CMAKE_${_lang}_FLAGS "${CMAKE_${_lang}_FLAGS} -Wno-deprecated-non-prototype")

    endif()
endforeach()

###
### Display user options and CMake configuration
###

message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME} (Processor: ${CMAKE_SYSTEM_PROCESSOR})")
message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER} (ID: ${CMAKE_C_COMPILER_ID}, Version: ${CMAKE_C_COMPILER_VERSION})")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER} (ID: ${CMAKE_CXX_COMPILER_ID}, Version: ${CMAKE_CXX_COMPILER_VERSION})")
message(STATUS "CMAKE_Fortran_COMPILER: ${CMAKE_Fortran_COMPILER} (ID: ${CMAKE_Fortran_COMPILER_ID}, Version: ${CMAKE_Fortran_COMPILER_VERSION})")
message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_Fortran_FLAGS: ${CMAKE_Fortran_FLAGS}")

mdsplus_print_options()

###
### RPATH Configuration, to remove dependence on [DY]LD_LIBRARY_PATH
###

file(RELATIVE_PATH
    _rpath_relative
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

if(APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    list(APPEND CMAKE_INSTALL_RPATH
        "@loader_path"
        "@executable_path/${_rpath_relative}"
        ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}
    )
else()
    list(APPEND CMAKE_INSTALL_RPATH
        "\${ORIGIN}"
        "\${ORIGIN}/${_rpath_relative}"
    )
endif()

# Automatically add libraries we depend on to RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)

###
### Dependencies
###

# CMake built-in packages to find

# Do not prefer system apple python
set(Python_FIND_FRAMEWORK LAST)

# In order for find_package(Python) to use the python specified with -DPYTHON or $PYTHON
# we need to convert it to an absolute path
if(NOT DEFINED Python_EXECUTABLE AND NOT PYTHON STREQUAL "")
    find_program(Python_EXECUTABLE "${PYTHON}")
    mark_as_advanced(Python_EXECUTABLE)
endif()

find_package(Python COMPONENTS Interpreter Development.Embed REQUIRED)

# Finds the platform specific threading libraries, usually pthreads
find_package(Threads REQUIRED)

# Finds the POSIX iconv() family of functions, which might be built into libc
find_package(Iconv REQUIRED)

# Finds the ZLIB compression library
find_package(ZLIB REQUIRED)

set(LibXml2_ROOT_DIR "${LIBXML2_DIR}")
mark_as_advanced(LibXml2_ROOT_DIR)
find_package(LibXml2 REQUIRED)

if(NOT NO_SYSTEM_BLAS)
    # Finds the Basic Linear Algebra Subprograms fortran functions
    find_package(BLAS)
    if(NOT TARGET BLAS::BLAS)
        message(SEND_ERROR "Unable to find BLAS, if no package is available for your system, use -DNO_SYSTEM_BLAS=ON")
    endif()
endif()

# Finds the LZMA compression library
find_package(LibLZMA)

# Custom packages to find

# cmake/FindLibM.cmake
find_package(LibM REQUIRED)

# cmake/FindReadline.cmake
set(Readline_ROOT_DIR "${READLINE_DIR}")
mark_as_advanced(Readline_ROOT_DIR)
find_package(Readline REQUIRED)

# cmake/FindDC1394.cmake
find_package(DC1394)

# Configuration-dependent dependencies

if(ENABLE_MOTIF)
    # cmake/FindMotif.cmake
    find_package(Motif REQUIRED)
endif()

if(ENABLE_SYBASE)
    # cmake/FindSybase.cmake
    find_package(Sybase REQUIRED)
endif()

if(ENABLE_HDF5)
    # cmake/FindHDF5.cmake
    set(HDF5_ROOT_DIR "$ENV{HDF5_DIR}")
    find_package(HDF5 REQUIRED)
endif()

if(ENABLE_JAVA)

    if(APPLE AND EXISTS /usr/libexec/java_home AND JAVA_HOME STREQUAL "")
        # This is a workaround for broken FindJNI.cmake on MacOS
        # which only likes EXPLICIT setting of JAVA_HOME
        # but also allows you to pick the jdk version
        execute_process(
            COMMAND /usr/libexec/java_home -v ${JAVA_VERSION}
            OUTPUT_VARIABLE _result
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )

        if(_result AND IS_DIRECTORY "${_result}")
            set(JAVA_HOME ${_result})
        endif()
    endif()

    find_package(Java ${JAVA_VERSION} COMPONENTS Runtime Development REQUIRED)
    find_package(JNI REQUIRED)

    include(UseJava)

    find_program(mvn_EXECUTABLE mvn)
    mark_as_advanced(mvn_EXECUTABLE)

    list(APPEND CMAKE_JAVA_COMPILE_FLAGS -encoding UTF-8)

endif()

if(ENABLE_IDL)

    if(IDL_DIR)
        set(IDL_ROOT_DIR "${IDL_DIR}")
    else()
        set(IDL_ROOT_DIR
            /usr/local/harris/idl
            /usr/local/exelis/idl
            /usr/local/itt/idl
            /usr/local/rsi/idl
            ${_3rd_party_apis}/idl
        )
    endif()

    # cmake/FindIDL.cmake
    find_package(IDL REQUIRED)

endif()

if(ENABLE_D3D)

    find_library(
        D3
        NAMES d3
        PATHS $ENV{D3DLIB_PATH}
        REQUIRED
    )

    mark_as_advanced(D3)

endif()

if(GSI)

    string(REPLACE ":" ";" _gsi_list "${GSI}")
    list(LENGTH _gsi_list _gsi_list_length)

    if(_gsi_list_length LESS 2)
        set(Globus_FLAVOR ${_gsi_list})
    else()
        list(GET _gsi_list 0 Globus_ROOT_DIR)
        list(GET _gsi_list 1 Globus_FLAVOR)
    endif()

    # (SLW) TODO: pthr???

    # cmake/FindGlobus.cmake
    find_package(Globus REQUIRED)

    # Only used by Roam, which is only enabled with Globus
    find_package(CURL REQUIRED)

endif()

if(BUILD_TESTING)

    find_library(LIBRT rt)
    mark_as_advanced(LIBRT)

    if(LIBRT)
        add_compile_definitions(HAVE_LIBRT)
    endif()

endif()

if(ENABLE_VALGRIND)

    # cmake/FindValgrind.cmake
    set(Valgrind_ROOT_DIR "${VALGRIND_LIB_DIR}")
    string(REPLACE "," ";" Valgrind_REQUESTED_TOOL_LIST "${VALGRIND_TOOLS}")
    find_package(Valgrind REQUIRED)

    if(CMAKE_C_COMPILER_ID STREQUAL "Clang"
        AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL "14"
        AND Valgrind_VERSION VERSION_LESS "3.20")

        message(STATUS "Setting ${_force_dwarf4} due to clang >= 14, and valgrind < 3.20")

        set(_force_dwarf4 "-fdebug-default-version=4")
        mdsplus_add_compiler_flags(${_force_dwarf4})
    endif()

    set(Valgrind_FLAGS
        --gen-suppressions=all
        --keep-debuginfo=yes
        --num-callers=64
        --trace-children=yes
        --child-silent-after-fork=yes
        --trace-children-skip-by-arg='*SetMdsplusFileProtection*'
    )

    set(Valgrind_memcheck_FLAGS
        --leak-check=full
        --show-reachable=no
    )

    set(Valgrind_helgrind_FLAGS
        --history-level=full
    )

    set(Valgrind_SUPPRESSION_FILES
        ${CMAKE_SOURCE_DIR}/conf/valgrind.supp/mdsplus.supp
    )

endif()

###
### Check for includes, Libraries, and Symbols
###

include(CheckIncludeFiles)
include(CheckSymbolExists)

# POSIX
check_include_files("alloca.h" HAVE_ALLOCA_H) # TODO: Remove?
check_include_files("grp.h" HAVE_GRP_H)
check_include_files("pthread.h" HAVE_PTHREAD_H)
check_include_files("pwd.h" HAVE_PWD_H)
check_include_files("sys/resource.h" HAVE_SYS_RESOURCE_H)
check_include_files("sys/types.h" HAVE_SYS_TYPES_H)
check_include_files("unistd.h" HAVE_UNISTD_H)
check_include_files("sys/filio.h" HAVE_SYS_FILIO_H)
check_symbol_exists(clock_gettime "time.h" HAVE_CLOCK_GETTIME)
check_symbol_exists(fork "unistd.h" HAVE_FORK)
check_symbol_exists(getaddrinfo "netdb.h" HAVE_GETADDRINFO)
check_symbol_exists(getpwuid "pwd.h" HAVE_GETPWUID)
check_symbol_exists(getrusage "sys/resource.h" HAVE_GETRUSAGE)
check_symbol_exists(mkstemp "stdlib.h" HAVE_MKSTEMP)
check_symbol_exists(pthread_lock_global_np "pthread.h" HAVE_PTHREAD_LOCK_GLOBAL_NP)

# FreeTDS / Sybase
set(HAVE_SYBASE ${Sybase_FOUND})

# VxWorks Real-Time Operating System
check_include_files("vxWorks.h" HAVE_VXWORKS_H)

# Readline
check_include_files("readline/readline.h" HAVE_READLINE_READLINE_H)
check_symbol_exists("rl_set_signals" "readline/readline.h" HAVE_RL_SET_SIGNALS)

# Linux Kernel
check_include_files("scsi/sg.h" HAVE_SCSI_SG_H)

###
### Generate _include/_mdsversion.h
###

configure_file(
    ${CMAKE_SOURCE_DIR}/_include/_mdsversion.h.in
    ${CMAKE_SOURCE_DIR}/_include/_mdsversion.h
)

###
### Generate include/mdsplus/mdsconfig.h
###

if(BUILD_SHARED_LIBS)
    set(SHARELIB_TYPE "${CMAKE_SHARED_LIBRARY_SUFFIX}")
else()
    set(SHARELIB_TYPE "${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()

# TODO HAVE_VISIBILITY
set(HAVE_VISIBILITY ON)

# This must be placed in the binary directory as it contains information
# specific to the OS/Compiler we're using
configure_file(
    ${CMAKE_SOURCE_DIR}/include/mdsplus/mdsconfig.h.in
    ${CMAKE_BINARY_DIR}/include/mdsplus/mdsconfig.h
)

###
### Ensure gen-* scripts have been run
###

include(MDSplusGenerateFiles)

mdsplus_generate_files(
    COMMAND ${Python_EXECUTABLE} deploy/gen-messages-exceptions.py
    GENERATED_FILES
    include/treeshr_messages.h
    include/tdishr_messages.h
    include/mdsshr_messages.h
    include/tcl_messages.h
    include/camshr_messages.h
    include/servershr_messages.h
    include/mitdevices_messages.h
    include/mdsdcl_messages.h
    python/MDSplus/mdsExceptions.py
    mdsshr/MdsGetStdMsg.c
    java/mdsplus-api/src/main/java/mds/MdsException.java
)

mdsplus_generate_files(
    COMMAND ${Python_EXECUTABLE} deploy/gen-include-opcbuiltins.py
    GENERATED_FILES
    include/opcbuiltins.h
)

mdsplus_generate_files(
    COMMAND ${Python_EXECUTABLE} deploy/gen-python-compound.py
    GENERATED_FILES
    python/MDSplus/compound.py
)

mdsplus_generate_files(
    COMMAND ${Python_EXECUTABLE} deploy/gen-tdishr-TdiHash.py
    GENERATED_FILES
    tdishr/TdiHash.c
)

mdsplus_generate_files(
    COMMAND ${Python_EXECUTABLE} deploy/gen-yacc-lex.py
    GENERATED_FILES
    mdsdcl/cmdParse.c
    mdsdcl/cmdParseLex.c
    mdsdcl/dcllex.h
    mdsdcl/dclyacc.h
    mdsdcl/mdsdclDeltatimeToSeconds.c
    mdsdcl/ocldToXml.c
    tdishr/TdiLex.c
    tdishr/tdilex.h
    tdishr/TdiYacc.c
    tdishr/tdiyacc.h
    treeshr/TreeFindNodeWild.c
)

###
### Generate Documentation
###

find_package(Doxygen)

if(Doxygen_FOUND)

    add_custom_target(
        docs
    )

endif()

###
### Libraries, Executables
###

# Helper variables for setting file permissions during install()

set(MODE_777
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_WRITE GROUP_EXECUTE
    WORLD_READ WORLD_WRITE WORLD_EXECUTE
)

set(MODE_755
    OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ             GROUP_EXECUTE
    WORLD_READ             WORLD_EXECUTE
)

set(MODE_644
    OWNER_READ OWNER_WRITE
    GROUP_READ
    WORLD_READ
)

# Some libraries need both a Shared and a Static version, this includes a macro that helps streamline
# that and create a second library target with the suffix `-static`. For `MdsShr` for example, it will
# create the target `MdsShr-static`, which could then produce `libMdsShr.a`. If BUILD_SHARED_LIBS=OFF,
# then only the static libraries are built, and the `-static` targets will be ALIAS targets instead.
include(MDSplusAddStaticCopy)

# Fortran Libraries

add_subdirectory(math)
add_subdirectory(d3dshr)

# Core Libraries

add_subdirectory(mdsshr)
add_subdirectory(treeshr) # Requires MdsShr
add_subdirectory(xtreeshr) # Requires TreeShr
add_subdirectory(tdishr) # Requires TreeShr
add_subdirectory(mdstcpip) # Requires TdiShr
add_subdirectory(mdslib) # Requires TdiShr
add_subdirectory(xmdsshr) # Requires TdiShr
add_subdirectory(mdsmisc) # Requires XTreeShr
add_subdirectory(mdsdcl) # Requires MdsIpShr
add_subdirectory(servershr) # Requires libMdsdcl
add_subdirectory(mdssql)
# add_subdirectory(mdsvme)

# Language Binding Libraries

add_subdirectory(mdslibidl) # Requires TdiShr
add_subdirectory(javamds) # Requires TdiShr
add_subdirectory(mdsobjects/cpp) # Requires JavaMds
add_subdirectory(mdsobjects/labview) # Requires MdsObjectsCppShr
add_subdirectory(python)
add_subdirectory(java)

# Command Line Tools

add_subdirectory(tdic)
add_subdirectory(tditest)
add_subdirectory(setevent)
add_subdirectory(wfevent)
# actlog is in actions/

# Graphical Tools

add_subdirectory(actions)
add_subdirectory(dwscope)
add_subdirectory(traverser)
add_subdirectory(idlmdsevent)
add_subdirectory(idlmdswidgets)

# Device Support

add_subdirectory(mitdevices)
add_subdirectory(camshr)
add_subdirectory(remcam) # Requires CamShr
add_subdirectory(pydevices)

# libMdsdcl commands

add_subdirectory(tcl)
add_subdirectory(ccl) # Requires CamShr
# cts_commands is in camshr/

# Globus

add_subdirectory(roam)
# MdsIpGSI is in mdstcpip/

# Configuration

add_subdirectory(rpm)
add_subdirectory(scripts)
add_subdirectory(manpages)

# Examples

add_subdirectory(examples/demoadc)

if(BUILD_TESTING)

    ###
    ### Testing
    ###

    if(GENERATE_VSCODE_LAUNCH_JSON)
        message(STATUS "Generating .vscode/launch.json")
    endif()

    include(MDSplusAddTest)

    if(ENABLE_SELFTEST)
        add_subdirectory(testing/selftest)
    endif()

    add_subdirectory(testing)
    add_subdirectory(mdsshr/testing)
    add_subdirectory(math/testing)
    add_subdirectory(treeshr/testing)
    add_subdirectory(mdslib/testing)
    add_subdirectory(mdstcpip/testing)
    add_subdirectory(tditest/testing)
    add_subdirectory(mdsobjects/cpp/testing)
    add_subdirectory(python/MDSplus/tests)

    add_subdirectory(java/tests) # defines JAVA_TEST_ENV_MODS, JAVA_TEST_JAR_LIST
    add_subdirectory(java/mdsobjects/tests)
    add_subdirectory(java/mdsplus-api/tests)

    if(GENERATE_VSCODE_LAUNCH_JSON)
        message(STATUS "Setting GENERATE_VSCODE_LAUNCH_JSON=OFF, set it again to regenerate .vscode/launch.json")
        # Disable this option after it has been set, otherwise it bogs down every Configure
        set(GENERATE_VSCODE_LAUNCH_JSON OFF CACHE BOOL "" FORCE)
    endif()

endif()

if(Motif_FOUND)

    ###
    ### Process *.uil files
    ###

    file(GLOB_RECURSE
        _uil_list
        "actions/*.uil"
        "dwscope/*.uil"
        "traverser/*.uil"
        "mitdevices/*.uil"
        "idlmdswidgets/*.uil"
        "xmdsshr/*.uil"
    )

    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/uid)

    foreach(_uil_file IN LISTS _uil_list)
        cmake_path(GET _uil_file STEM LAST_ONLY _stem)
        set(_uid_file ${CMAKE_CURRENT_BINARY_DIR}/uid/${_stem}.uid)

        add_custom_command(
            OUTPUT ${_uid_file}
            DEPENDS ${_uil_file}
            COMMAND ${uil_EXECUTABLE} -I${CMAKE_SOURCE_DIR}/include -o ${_uid_file} ${_uil_file}
        )

        LIST(APPEND _uid_file_list ${_uid_file})
    endforeach()

    add_custom_target(
        generate-uids ALL
        DEPENDS ${_uid_file_list}
    )

    install(
        DIRECTORY ${CMAKE_BINARY_DIR}/uid
        DESTINATION .
    )

endif()

###
### ChangeLog
###

find_package(Git)

if(GIT_FOUND)

git(GIT_CHANGELOG log --decorate=full --no-merges)

file(WRITE ${CMAKE_BINARY_DIR}/ChangeLog "${GIT_CHANGELOG}")

install(
    FILES ${CMAKE_BINARY_DIR}/ChangeLog
    DESTINATION .
)

endif()

###
### Install the remaining files and directories
###

install(
    DIRECTORY
        desktop
        epics
        idl
        include
        LabView
        matlab
        nodejs
        php
        pixmaps
        pydevices
        tdi
        trees
        xml
        ${CMAKE_BINARY_DIR}/include
    DESTINATION .
    USE_SOURCE_PERMISSIONS

    PATTERN "*.php" PERMISSIONS ${MODE_755}
    PATTERN "*.py" PERMISSIONS ${MODE_755}
    PATTERN "*.fun" PERMISSIONS ${MODE_755}

    PATTERN "*.in" EXCLUDE # Templates for generated files
    PATTERN "Makefile" EXCLUDE # TODO: Remove
    PATTERN "Makefile.*" EXCLUDE # TODO: Remove
    PATTERN "CMakeLists.txt" EXCLUDE

    PATTERN "*.pyc" EXCLUDE
    PATTERN "__pycache__" EXCLUDE
)

install(
    FILES
        MDSplus-License.rtf
        MDSplus-License.txt
    DESTINATION .
)

install(
    FILES
        setup.csh
        setup.sh
    DESTINATION .
    PERMISSIONS ${MODE_755}
)

install(
    DIRECTORY
    DESTINATION local/tdi
)