
###
### DemoAdcShr
###

add_library(
    DemoAdcShr
    DemoAdc.c
)

install(TARGETS DemoAdcShr)

add_static_copy(DemoAdcShr)