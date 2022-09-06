
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

if(BLAS_FOUND)

    target_link_libraries(
        MdsMath
        PUBLIC
            BLAS::BLAS
    )

else()

    message("Unable to find libblas, using local copy")

    target_sources(
        MdsMath
        PRIVATE
            caxpy.f
            daxpy.f
            saxpy.f
            zaxpy.f
            cgeco.f
            dgeco.f
            sgeco.f
            zgeco.f
    )

endif()

# (SLW) TODO: Check compatible fortran?
target_compile_options(
    MdsMath
    PRIVATE
        $<$<COMPILE_LANGUAGE:Fortran>:
            -Wno-maybe-uninitialized
            -Wno-compare-reals
        >
)

target_compile_definitions(
    MdsMath
    PRIVATE
        -DLIBPREFIX=MdsMath
)

install(
    TARGETS MdsMath
)