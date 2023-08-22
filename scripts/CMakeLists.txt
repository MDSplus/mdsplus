
###
### Scripts
###

set(_script_list
    job_finish
    job_functions
    job_output
    job_que
    job_start
    job_wait
    MonitorCpciData.py
    remote_submit
    SetMdsplusFileProtection
    synchronize_unix
    unix_submit
)

file(COPY ${_script_list}
    DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

install(
    FILES ${_script_list}
    TYPE BIN
    PERMISSIONS ${MODE_755}
)