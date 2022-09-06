
MACRO(GET_DATE _output_variable)
    FIND_PROGRAM(DATE_EXECUTABLE date)
    MARK_AS_ADVANCED(DATE_EXECUTABLE)
    IF(DATE_EXECUTABLE)
        EXECUTE_PROCESS(
            COMMAND "${DATE_EXECUTABLE}"
            OUTPUT_VARIABLE ${_output_variable}
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    ELSE()
        # STRING(TIMESTMAP) is missing AM/PM and the Timezone, so use a sensible fallback
        STRING(TIMESTAMP ${_output_variable} "%a %d %b %Y %H:%M:%S UTC" UTC)
    ENDIF()
ENDMACRO()
