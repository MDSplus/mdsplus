@START "" /WAIT /D "%MDSPLUS_DIR%\java\classes" javaw -Xss5M ^
 -cp "mdsplus-api.jar;jTraverser2.jar;DeviceBeans.jar;jDevices.jar" ^
 mds.devices.Device %*
