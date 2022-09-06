
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

install(TARGETS MdsMisc)

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

add_static_copy(MdsMisc)
