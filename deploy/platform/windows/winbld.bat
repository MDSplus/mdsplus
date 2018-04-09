call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
set VisualStudioVersion=15.0
devenv %~dp0..\mdsobjects\MdsObjectsCppShr-VS.sln /build "Release|X64"
devenv %~dp0..\mdsobjects\MdsObjectsCppShr-VS.sln /build "Release|win32"
