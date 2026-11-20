
add_subdirectory(MDSplus)

mdsplus_option(
    ENABLE_PYTHON_LOCAL_WHEEL BOOL
    "Build a pip-installable 'local' wheel whose only payload is a .pth file that points at the installed python/MDSplus directory."
    DEFAULT ON
)

if(ENABLE_PYTHON_LOCAL_WHEEL)
    add_subdirectory(local_wheel)
endif()
