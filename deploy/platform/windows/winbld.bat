call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\vcvars32.bat"
set VisualStudioVersion=12.0
devenv %~dp0..\mdsobjects\MdsObjectsCppShr-VS.sln /build "Release|X64"
devenv %~dp0..\mdsobjects\MdsObjectsCppShr-VS.sln /build "Release|win32"
