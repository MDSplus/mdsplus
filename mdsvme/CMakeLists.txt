
###
### MdsVme
###

add_library(
    MdsVme
    mdsvme.c
)

mdsplus_add_static_copy(MdsVme _static_target)

# install(TARGETS MdsVme ${_static_target})