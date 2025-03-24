
# Overview

This is temporary documentation for building the port of MDSplus on MacOS.  It focuses on the Apple Silicon version but also has some information about the Intel version.

# MacOS Apple Silicon vs. Intel

The term, *Apple Silicon*, denotes the "M" series CPUs which have been used in Apple computers since 2020.  They are Apple's customized version of Arm Ltd.'s CPUs, which are commonly called the "arm64" architecture (but officially named by Arm as the "aarch64").

With the introduction of Apple Silicon CPUs, Apple also changed the *Application Binary Interface* (ABI) compared to the previous MacOS Intel (aka x86-64) systems.

On x86-64, it doesn't matter whether ...
1) a procedure declaration consists of a fixed number of parameters, 
2) or whether it has a variable number of parameters.

The parameters are passed the same way to both types of functions.

On Apple Silicon, Apple decided to instead do this ...
- for 1) above, parameters are passed in registers,
- for 2), they are passed on the stack.

The issue is that if a function is declared as stack based, but is called from another source file that redefined the function as register based, then the parameters aren't passed correctly which results in exceptions and segfaults.   The same also applies for the opposite case, namely the function is register based and called as stack based.

This is a very big deal for MDSplus, because it frequently passes pointers to functions which are then executed. This works well on the x86-64 architecture used by Linux, Windows and MacOS (Intel).

On Apple Silicon, MDSplus can't tell whether a function pointer resolves to a procedure that is register based or stack based.   Thus it is a challenge to port MDSplus to Apple Silicon.  The solution is to use the `libffi` (Foreign Function Interface) that ships with MacOS.

For more details, refer to this Apple documentation.  In particular, read the section about *Don't Redeclare a Function to Have Variable Parameters*.
[Architectural Differences]( https://developer.apple.com/documentation/apple-silicon/addressing-architectural-differences-in-your-macos-code )


# Tool Chain for MDSplus on MacOS

The Apple Silicon port was built with these tools.

- MacOS Sequoia 15.3.2
- flang-new 16.0.6 -- an LLVM compiler for Fortran
- clang 16.0.6 -- an LLVM compiler for C / C++
- Python 3.12.8 -- part of Apple's Xcode framework
    - numpy -- also install the associated version
    - used by CMake and MDSplus
- Python 2.7.18 and numpy
    - used by many MDSplus "devices"
    - installed from MacPorts
    - *NOTE: the Brew package manager installed the x86-64 versions, which of course don't run on Apple Silicon, which is why MacPorts was used instead
- libffi -- included with MacOS
- Xcode 16.2 -- Apple's IDE installs many development libraries and tools
- openmotif 2.3.8_3 -- from MacPorts
- other packages -- see the full MacPorts list at the end of this document


# Tips

Before running CMake and building, do these steps:

```
export FC=<dir to flang-new>
export CC=<dir to clang>   # MDSplus port not yet working with Apple's Clang
export PyLib=/opt/local/lib/libpython2.7.dylib
```

# Caveats

The Apple Silicon port involved many source changes, thus should be viewed as experimental.  Although it passes 100% of the automated test suite, there are likely some scenarios that will bump into the register vs. stack issue, thus resulting in exceptions and segfaults.

Here are some known issues:
- Not all APIs have been thoroughly tested
    - Lightly Tested: MATLAB, IDL, others?
    - Tested: C/C++, Python, Java, Fortran, Tdic
- TDI expressions that call a function in an external library
    - If the function is variadic, the number of fixed arguments must be specified using the `<function>#<number_of_fixed_arguments>` notation
        - For example, `"TdiShr->TdiPi#1(val(0),val(1))"` specifies one fixed argument
        - An equivalent statement is `"BUILD_CALL(8, "TdiShr", "TdiPi#1", val(0), val(1))"`
    - Functions that are not variadic can be called as usual (i.e., without `#<num_fixed_agrs>`)
        - For example, `"MyLibrary->MyFunction()"`
- Device methods
    - MDSplus device methods have not been ported or tested.
    - However, it is unlikely that customers will use MacOS MDSplus for a data acquisition server, thus device support is probably not necessary.   (The MacOS version will be used primarily as client software.)
    - Device methods will be ported to Apple Silicon on an "as needed" basis (i.e., in response to bug reports filed by customers).
- Action Servers
    - Segfaults might arise when an "action server" executes a job that involves calling a function in an external library.
    - Although the server code has been ported to Apple Silicon, external functions might cause problems.
    - However, it is unlikely that MDSplus (Apple Silicon) will be used as an "action server".
    - Note that the `LibCallg()` and `LibCallgFfi()` functions that call external libraries have these limitations:
        - parameters can be passed "by reference" or "by value"
        - the "by value" parameters must not exceed the "size" of a pointer
        - external routines that have parameters that are intricate data structures (structs, objects) pose challenges because TDI likely won't be able to create the required data structures
        - users will have to write a "shim" function if they wish to call external functions that do not conform to the above limitations
        - note that these limitations apply to all versions of MDSplus; they are not specific to Apple Silicon
- Motif applications
    - The "traverser" application testing was very minimal (opening a tree, displaying a node, modifying data).
    - The `dwscope` application was merely launched and displayed the plasma current from a single C-Mod shot.   No additional testing was done.
    - No device setup forms written in Motif were tested.
    - Because of the very light testing, there is no guarantee that the Motif portions of MDSplus will work well on Apple Silicon.
- jScope
    - On the `cmake` branch, the `MDSIP_CLIENT_LOCAL_LOGFILE` environment variable must be defined in order to run `jScope`.   That is because the `mdsip-client-local` script was modified on the `cmake` branch, but not on the `alpha` branch.   Note that the modification is related to the testing suite on the `cmake` branch and is unrelated to the Apple Silicon port.   (The environment variable was introduced prior to porting MDSplus to Apple Silicon.)
- IDL
    - IDL ignores the `DYLD_LIBRARY_PATH` environment variable, thus cannot find the various MDSplus `*.dylib` files when IDL is run from its default working directory.   
    - Two workaround have been found:
        - create this soft link: `ln -s $MDSPLUS_DIR/lib /Applications/NV5/idl90/bin/lib`,
        - or always `cd $MDSPLUS_DIR/lib` before running IDL
    - IDL likely is compiled with `RPATH`, thus expects external libraries to be in the following directories:
```
IDL> mdsconnect, 'localhost'
% Compiled module: MDSCONNECT.
% Compiled module: MDS_KEYWORD_SET.
% Compiled module: MDSDISCONNECT.
% CALL_EXTERNAL: Error loading sharable executable.
                 Symbol: IdlConnectToMds, File = libMdsIpShr.dylib
                 dlopen(libMdsIpShr.dylib, 0x0001): tried: 
                  '//Applications/NV5/idl90/bin/bin.darwin.arm64/libMdsIpShr.dylib' (no such file), 
                  'libMdsIpShr.dylib' (no such file), 
                  '/System/Volumes/Preboot/Cryptexes/OSlibMdsIpShr.dylib' (no such file),
                  '/Applications/NV5/idl90/bin/bin.darwin.arm64/libMdsIpShr.dylib' (no such file),
                  '/Applications/NV5/idl90/bin/bin.darwin.arm64/../libMdsIpShr.dylib' (no such file),
                  '/Applications/NV5/idl90/bin/bin.darwin.arm64/../lib/libMdsIpShr.dylib' (no such file),
                  '/Applications/NV5/idl90/bin/bin.darwin.arm64/libMdsIpShr.dylib' (no such file),
                  '/Applications/NV5/idl90/bin/bin.darwin.arm64/../libMdsIpShr.dylib' (no such file),
                  '/Applications/NV5/idl90/bin/bin.darwin.arm64/../lib/libMdsIpShr.dylib' (no such file), 
                  '/usr/lib/libMdsIpShr.dylib' (no such file, not in dyld cache), 
                  'libMdsIpShr.dylib' (no such file), 
                  '/usr/lib/libMdsIpShr.dylib' (no such file, not in dyld cache)
% Execution halted at: $MAIN$          
IDL> 
``` 
- mdsip services
    - To automatically run a mdsip service when MacOS boots up, it is necessary to create a "Property List" file (aka `plist` file) so that `launchd` can start the service.   The `plist` file still has to be written, tested and added to the MacOS versions of MDSplus (i.e. applies to both Apple Silicon and Intel).
    - For more information, refer to this Apple Developer documentation: [Daemons and Services](https://developer.apple.com/library/archive/documentation/MacOSX/Conceptual/BPSystemStartup/Chapters/CreatingLaunchdJobs.html).  Note that the bottom of the web page lists the constraints / requirements that a process must abide by in order to be run by `launchd`.

# MacPorts packages

This is the full list of MacPorts packages installed on the M2 MacBook Pro used to develop the Apple Silicon port of MDSplus.   Note that many of these packages are probably unnecessary.

```
The following ports are currently installed:
  autoconf @2.71_1
  autoconf @2.71_2
  autoconf @2.72_0 (active)
  automake @1.16.5_0
  automake @1.17_0 (active)
  bash @5.2.15_0
  bash @5.2.37_0 (active)
  bison @3.8.2_2 (active)
  bison-runtime @3.8.2_0 (active)
  boehmgc @8.2.4_0+universal
  boehmgc @8.2.8_0+universal (active)
  brotli @1.0.9_2
  brotli @1.1.0_0 (active)
  bzip2 @1.0.8_0 (active)
  cctools @949.0.1_3+xcode (active)
  clang-16 @16.0.6_1+analyzer
  clang-16 @16.0.6_2+analyzer
  clang-16 @16.0.6_6+analyzer (active)
  clang_select @2.2_1
  clang_select @2.4_0 (active)
  cmake @3.24.4_0
  cmake @3.31.3_0 (active)
  cmake-bootstrap @3.9.6_0 (active)
  curl @8.2.1_0+http2+ssl
  curl @8.3.0_0+http2+ssl
  curl @8.11.1_0+brotli+http2+idn+psl+ssl+zstd (active)
  curl-ca-bundle @8.2.1_0
  curl-ca-bundle @8.3.0_0
  curl-ca-bundle @8.11.1_0 (active)
  cython_select @0.1_2 (active)
  db48 @4.8.30_5 (active)
  diffutils-for-muniversal @3.8_0 (active)
  doxygen @1.9.3_3
  doxygen @1.11.0_0 (active)
  expat @2.5.0_0
  expat @2.5.0_0+universal
  expat @2.6.4_0+universal (active)
  fftw-3 @3.3.10_0+gfortran (active)
  flang-16 @16.0.6_0
  flang-16 @16.0.6_1
  flang-16 @16.0.6_2 (active)
  flex @2.6.4_0 (active)
  fontconfig @2.14.2_0
  fontconfig @2.15.0_0 (active)
  freetds @1.3.18_0
  freetds @1.3.19_0
  freetds @1.3.20_0
  freetds @1.4.2_0
  freetds @1.4.24_0 (active)
  freetype @2.12.1_0
  freetype @2.13.3_0 (active)
  gcc12 @12.3.0_0+stdlib_flag
  gcc12 @12.3.0_2+stdlib_flag (active)
  gcc12-libcxx @12.3.0_0+clang14
  gcc12-libcxx @12.3.0_2+clang14
  gcc12-libcxx @12.4.0_1+clang14 (active)
  gcc14 @14.2.0_3+stdlib_flag (active)
  gcc14-libcxx @14.2.0_3+clang18 (active)
  gcc_select @0.1_10 (active)
  gdbm @1.23_0
  gdbm @1.24_0 (active)
  gettext @0.21.1_0
  gettext @0.22.5_0 (active)
  gettext-runtime @0.21.1_0
  gettext-runtime @0.21.1_0+universal
  gettext-runtime @0.22.5_0+universal (active)
  gettext-tools-libs @0.21.1_0
  gettext-tools-libs @0.22.5_0 (active)
  gmake @4.4.1_0 (active)
  gmp @6.2.1_1
  gmp @6.2.1_1+universal
  gmp @6.3.0_0+universal (active)
  gnutls @3.7.9_3
  gnutls @3.7.10_0
  gnutls @3.7.11_1 (active)
  gperf @3.1_0 (active)
  help2man @1.49.3_2+perl5_34 (active)
  icu @73.2_0
  icu @76.1_0 (active)
  isl @0.24_1 (active)
  kerberos5 @1.21.1_1
  kerberos5 @1.21.2_0
  kerberos5 @1.21.3_0 (active)
  ld64 @3_4+ld64_xcode
  ld64 @3_6+ld64_xcode (active)
  ld64-xcode @2_4
  ld64-xcode @2_6 (active)
  libarchive @3.7.0_0
  libarchive @3.7.1_0
  libarchive @3.7.2_0
  libarchive @3.7.7_0 (active)
  libatomic_ops @7.8.0_0+universal (active)
  libb2 @0.98.1_1 (active)
  libcomerr @1.47.0_0
  libcomerr @1.47.1_0 (active)
  libcxx @5.0.1_5 (active)
  libedit @20221030-3.1_0
  libedit @20221030-3.1_0+universal
  libedit @20230828-3.1_0+universal
  libedit @20240808-3.1_0+universal (active)
  libffi @3.4.4_0
  libffi @3.4.6_1 (active)
  libgcc @6.0_0
  libgcc @9.0_0 (active)
  libgcc12 @12.3.0_0+stdlib_flag
  libgcc12 @12.3.0_2+stdlib_flag
  libgcc12 @12.4.0_1+stdlib_flag (active)
  libgcc13 @13.3.0_2+stdlib_flag (active)
  libgcc14 @14.2.0_3+stdlib_flag (active)
  libiconv @1.17_0
  libiconv @1.17_0+universal (active)
  libidn2 @2.3.4_1
  libidn2 @2.3.7_0 (active)
  libjpeg-turbo @2.1.5.1_0 (active)
  libmpc @1.3.1_0 (active)
  libomp @16.0.6_0
  libomp @19.1.6_0 (active)
  libpng @1.6.40_0
  libpng @1.6.45_0 (active)
  libpsl @0.21.2-20230117_0
  libpsl @0.21.5-20240306_0 (active)
  libtasn1 @4.19.0_0 (active)
  libtextstyle @0.21.1_0
  libtextstyle @0.22.5_0 (active)
  libtool @2.4.7_0+universal
  libtool @2.4.7_1+universal
  libtool @2.5.3_0+universal (active)
  libunistring @1.1_0
  libunistring @1.3_0 (active)
  libuv @1.44.2_0 (active)
  libxml2 @2.10.4_2
  libxml2 @2.11.5_0
  libxml2 @2.13.5_2 (active)
  libxslt @1.1.37_2
  libxslt @1.1.38_0
  libxslt @1.1.42_0 (active)
  libyaml @0.2.5_0 (active)
  llvm-16 @16.0.6_0
  llvm-16 @16.0.6_1 (active)
  llvm_select @2_1 (active)
  lmdb @0.9.31_0
  lmdb @0.9.33_0 (active)
  lz4 @1.9.4_0
  lz4 @1.10.0_0 (active)
  lzip @1.24.1_0 (active)
  lzo2 @2.10_0 (active)
  m4 @1.4.19_1 (active)
  mlir-16 @16.0.6_0
  mlir-16 @16.0.6_1 (active)
  mpfr @4.2.0_0
  mpfr @4.2.1_0 (active)
  ncurses @6.4_0
  ncurses @6.4_0+universal
  ncurses @6.5_0+universal (active)
  nettle @3.9.1_0
  nettle @3.10_0 (active)
  nghttp2 @1.55.1_0
  nghttp2 @1.56.0_0
  nghttp2 @1.64.0_0 (active)
  OpenBLAS @0.3.23_0+gcc12+lapack
  OpenBLAS @0.3.24_0+gcc12+lapack (active)
  openjdk11 @11.0.20_0+release+server
  openjdk11 @11.0.20.1_0+release+server (active)
  openmotif @2.3.8_3 (active)
  openssl @3_11
  openssl @3_12
  openssl @3_13
  openssl @3_21 (active)
  openssl3 @3.1.1_0
  openssl3 @3.1.2_0
  openssl3 @3.1.3_0
  openssl3 @3.4.0_2 (active)
  openssl11 @1.1.1w_1 (active)
  ossp-uuid @1.6.2_13+perl5_34 (active)
  p5.34-locale-gettext @1.70.0_1 (active)
  p11-kit @0.25.0_0
  p11-kit @0.25.5_0 (active)
  perl5 @5.34.1_0+perl5_34
  perl5 @5.34.3_0+perl5_34 (active)
  perl5.34 @5.34.1_0
  perl5.34 @5.34.3_1 (active)
  pkgconfig @0.29.2_0 (active)
  py-numpy @1.24.1_0
  py-numpy @1.25.2_0 (active)
  py27-cython @3.0.11_0 (active)
  py27-cython-compat @0.29.37_0 (active)
  py27-numpy @1.16.6_4+gfortran (active)
  py27-oldest-supported-numpy @0.1_0 (active)
  py27-setuptools @44.1.1_0 (active)
  py311-build @1.2.2.post1_0 (active)
  py311-cython @0.29.36_0 (active)
  py311-cython-compat @0.29.37_0 (active)
  py311-installer @0.7.0_0 (active)
  py311-numpy @1.24.1_0+gfortran+openblas
  py311-numpy @1.25.2_0+gfortran (active)
  py311-packaging @24.2_0 (active)
  py311-pygments @2.18.0_0 (active)
  py311-pyproject_hooks @1.2.0_0 (active)
  py311-setuptools @67.7.2_0
  py311-setuptools @68.1.2_0
  py311-setuptools @68.2.2_0
  py311-setuptools @75.8.0_0 (active)
  py311-wheel @0.45.1_0 (active)
  py311-yaml @6.0.2_0 (active)
  pygments_select @0.1_1 (active)
  python2_select @0.1_0 (active)
  python3_select @0.0_3
  python3_select @0.1_0 (active)
  python3_select-310 @0.1_1 (active)
  python3_select-311 @0.1_1 (active)
  python3_select-312 @0.1_1 (active)
  python27 @2.7.18_10+lto+optimizations (active)
  python310 @3.10.16_0+lto+optimizations (active)
  python311 @3.11.4_0+lto+optimizations
  python311 @3.11.5_0+lto+optimizations
  python311 @3.11.6_0+lto+optimizations
  python311 @3.11.11_0+lto+optimizations (active)
  python312 @3.12.8_0+lto+optimizations (active)
  python_select @0.3_10 (active)
  python_select-310 @0.3_1 (active)
  python_select-311 @0.3_1 (active)
  python_select-312 @0.3_1 (active)
  readline @8.2.001_0
  readline @8.2.013_0 (active)
  sqlite3 @3.42.0_0
  sqlite3 @3.42.0_0+universal
  sqlite3 @3.43.1_0+universal
  sqlite3 @3.43.1_1+universal
  sqlite3 @3.48.0_0+universal (active)
  texinfo @7.1_1+perl5_34 (active)
  xar @1.8.0.496_0
  xar @1.8.0.498_0
  xar @1.8.0.501_0 (active)
  Xft2 @2.3.8_0 (active)
  xorg-libice @1.1.1_0 (active)
  xorg-libpthread-stubs @0.4_0
  xorg-libpthread-stubs @0.5_0 (active)
  xorg-libsm @1.2.4_0 (active)
  xorg-libX11 @1.8.6_0 (active)
  xorg-libXau @1.0.11_0 (active)
  xorg-libxcb @1.15_0+python311
  xorg-libxcb @1.16_0+python311 (active)
  xorg-libXdmcp @1.1.4_0 (active)
  xorg-libXext @1.3.5_0 (active)
  xorg-libXmu @1.1.4_0 (active)
  xorg-libXp @1.0.4_0 (active)
  xorg-libXt @1.3.0_0 (active)
  xorg-xcb-proto @1.15.2_0+python311
  xorg-xcb-proto @1.16.0_0+python311 (active)
  xorg-xorgproto @2023.2_0 (active)
  xrender @0.9.11_0 (active)
  xz @5.4.3_0
  xz @5.4.4_0
  xz @5.6.3_0 (active)
  zlib @1.2.13_0
  zlib @1.2.13_0+universal
  zlib @1.3_0+universal
  zlib @1.3.1_0+universal (active)
  zstd @1.5.5_0
  zstd @1.5.6_0 (active)
```
