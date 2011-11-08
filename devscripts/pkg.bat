"C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
msbuild /p:Configuration="Release" /p:Platform="Java" mdsplus.sln
msbuild /p:Configuration="Release" /p:Platform="Win32" mdsplus.sln
msbuild  /p:Configuration="Release" /p:Platform="x64" mdsplus.sln
msbuild /p:Configuration="Release" /p:Setup32 mdsplus.sln
msbuild /p:Configuration="Release" /p:Setup64 mdsplus.sln
