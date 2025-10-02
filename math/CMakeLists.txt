
if(Fortran_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_Fortran_FLAGS
        "${CMAKE_Fortran_FLAGS} -Wno-maybe-uninitialized -Wno-compare-reals"
    )
elseif(Fortran_COMPILER_ID STREQUAL "Intel")
    set(CMAKE_Fortran_FLAGS
        "${CMAKE_Fortran_FLAGS} -warn noexternals -warn nodeclarations -warn notruncated_source"
    )
endif()

###
### MdsMath
###

add_library(
    MdsMath
    boxsmooth.c
    cmul.f
    csint.f
    csval.f
    dcsint.f
    dcsval.f
    dmul.f
    dpermu.f
    dppval.f
    dsrch.f
    dsvrgn.f
    gn.f
    isrch.f
    msceez.f
    mskrv1.f
    mskrv2.f
    msshch.f
    mstrms.f
    permu.f
    ppval.f
    smul.f
    srch.f
    ssrch.f
    svign.f
    svrgn.f
    toms510.f
    umach.f
    zmul.f
)

if(NOT USE_SYSTEM_BLAS)

    message(STATUS "Using local copy of blas")

    target_sources(
        MdsMath
        PRIVATE
            blas/caxpy.f
            blas/daxpy.f
            blas/saxpy.f
            blas/zaxpy.f
            blas/cgeco.f
            blas/dgeco.f
            blas/sgeco.f
            blas/zgeco.f
    )

else()

    target_link_libraries(
        MdsMath
        PUBLIC
            BLAS::BLAS
    )

endif()

target_compile_definitions(
    MdsMath
    PRIVATE
        LIBPREFIX=MdsMath
)

install(TARGETS MdsMath)