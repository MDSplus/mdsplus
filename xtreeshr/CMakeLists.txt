
###
### XTreeShr
###

add_library(
    XTreeShr
    XTreeConvertToLongTime.c
    XTreeDefaultResample.c
    XTreeDefaultSquish.c
    XTreeGetSegmentList.c
    XTreeGetTimedRecord.c
)

target_link_libraries(
    XTreeShr
    PUBLIC
        TdiShr
)

target_compile_definitions(
    XTreeShr
    PRIVATE
        -DLIBPREFIX=XTree
)

mdsplus_add_static_copy(XTreeShr _static_target)

install(TARGETS XTreeShr ${_static_target})