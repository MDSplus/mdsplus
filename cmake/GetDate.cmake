# GetDate.cmake
#
# Get the current date/timestamp from the `date` executable if available, or fallback to string(TIMESTAMP)
#

macro(get_date _output_variable)
    find_program(date_EXECUTABLE date)
    mark_as_advanced(date_EXECUTABLE)
    if(date_EXECUTABLE)
        execute_process(
            COMMAND "${date_EXECUTABLE}"
            OUTPUT_VARIABLE ${_output_variable}
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    else()
        # STRING(TIMESTMAP) is missing AM/PM and the Timezone, so use a sensible fallback
        string(TIMESTAMP ${_output_variable} "%a %d %b %Y %H:%M:%S UTC" UTC)
    endif()
endmacro()
