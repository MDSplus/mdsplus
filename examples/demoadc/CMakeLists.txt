
###
### DemoAdcShr
###

add_library(
    DemoAdcShr
    DemoAdc.c
)

mdsplus_add_static_copy(DemoAdcShr _static_target)

# install(TARGETS ${_target_list})