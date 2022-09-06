
###
### */_version.py
###

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/MitDevices/_version.py.in
    ${CMAKE_CURRENT_SOURCE_DIR}/MitDevices/_version.py
)

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/RfxDevices/_version.py.in
    ${CMAKE_CURRENT_SOURCE_DIR}/RfxDevices/_version.py
)

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/W7xDevices/_version.py.in
    ${CMAKE_CURRENT_SOURCE_DIR}/W7xDevices/_version.py
)

CONFIGURE_FILE(
    ${CMAKE_CURRENT_SOURCE_DIR}/HtsDevices/_version.py.in
    ${CMAKE_CURRENT_SOURCE_DIR}/HtsDevices/_version.py
)