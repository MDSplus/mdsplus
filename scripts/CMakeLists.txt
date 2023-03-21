
###
### Scripts
###

install(
    FILES
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
    TYPE BIN
    PERMISSIONS ${MODE_755}
)