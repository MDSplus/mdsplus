
###
### MdsMisc
###

add_library(
    MdsMisc
    boxsmooth.c
    bwfilter.c
    getnidarray.c
    getreferences.c
    resample.c
    step_resample.c
    fformat.c
    filter.c
    complex.c
    butterworth.c
    bessel.c
    fir.c
    iir.c
    MdsFilter.c
    whoami.c
    ScopeUtilities.c
)

target_link_libraries(
    MdsMisc
    PUBLIC
        XTreeShr
)

target_compile_definitions(
    MdsMisc
    PRIVATE
        -DLIBPREFIX=MdsMisc
)

mdsplus_add_static_copy(MdsMisc _static_target)

install(TARGETS MdsMisc ${_static_target})