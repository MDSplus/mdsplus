
# MDSplus and CMake

MDSplus has moved build systems to CMake, the following guide should help you make the transition from `./configure` to `cmake`.

For any questions or concerns, please reach out to slwalsh@psfc.mit.edu

## Simple configuration

The simplest case for building MDSplus is the following:

```
# Make a directory to hold the intermediate objects and generated files
mkdir build

# Change to that directory
cd build

# Run CMake with a path to the source directory
cmake ..

# Build
make -j

# Install (to /usr/local/mdsplus)
sudo make install
```

## Advanced configuration

### Specifying configuration options

Almost every option from `./configure` is available from CMake. When (re-)configuring, specify the options like so:

```
cmake -DENABLE_XYZ=ON -DXYZ_PATH=/path/to/xyz .. 
```

For an interactive configuration complete with descriptions, default values, and dropdowns you can run `ccmake` or install the CMake GUI.

To list all options and their current values, use `cmake -LAH`.

The options from `./configure` and their CMake counterparts are listed below, along with some clarifications (`*`).

| `./configure`                           | CMake                                                   |
| --------------------------------------- | ------------------------------------------------------- |
| `--disable-shared`                      | `BUILD_SHARED_LIBS=OFF`                                 |
| `--enable-debug=no`                     | `CMAKE_BUILD_TYPE=Release`*                             |
| `--enable-debug=yes`                    | `CMAKE_BUILD_TYPE=Debug`*                               |
| `--enable-debug=info`                   | `CMAKE_BUILD_TYPE=RelWithDebInfo`*                      |
| `--enable-debug=profile`                | `ENABLE_GPROF=ON` and `CMAKE_BUILD_TYPE=RelWithDebInfo` |
| `--prefix=[PATH]`*                      | `CMAKE_INSTALL_PREFIX=[PATH]`*                          |
| `--exec-prefix=[PATH]`*                 | See below                                               |
| `--disable-largefile`                   | `ENABLE_LARGEFILE=OFF`                                  |
| `--enable-perf`                         | `ENABLE_PERF=ON`                                        |
| `--disable-java`                        | `ENABLE_JAVA=OFF`                                       |
| `--enable-d3d`                          | `ENABLE_D3D=ON`                                         |
| `--disable-xmltest`                     | `ENABLE_XMLTEST=OFF`                                    |
| `--enable-valgrind`                     | `ENABLE_VALGRIND=ON`                                    |
| `--enable-doxygen`                      | `ENABLE_DOXYGEN=ON`                                     |
| `--enable-werror`                       | `ENABLE_WERROR=ON`                                      |
| `--enable-wreturns`                     |                                                         |
| `--enable-wconversion`                  |                                                         |
| `--enable-sanitize=[FLAVOR]`            | `SANITIZE=[FLAVOR]`                                     |
| `--enable-silent-rules`                 |                                                         |
| `--disable-silent-rules`                |                                                         |
| `--jars`                                | See below                                               |
| `--with-jdk=[PATH]`                     | `JDK_DIR=[PATH]`                                        |
| `--with-java_target=[TARGET]`           |                                                         |
| `--with-java_bootclasspath=[CLASSPATH]` |                                                         |
| `--with-gsi=[LOCATION]:[FLAVOR]`        | `GSI=[LOCATION]:[FLAVOR]` or `GSI=[FLAVOR]`             |
| `--with-srb=[PATH]`                     | `SRB_DIR=[PATH]`                                        |
| `--with-labview=[PATH]`                 | `ENABLE_LABVIEW=ON` or `LABVIEW_DIR=[PATH]`             |
| `--with-idl=[PATH]`                     | `ENABLE_IDL=ON` or `IDL_DIR=[PATH]`*                    |
| `--with-readline=[PATH]`                | `READLINE_DIR=[PATH]`                                   |
| `--with-xml-prefix=[PATH]`              | `LIBXML2_DIR=[PATH]`                                    |
| `--with-xml-exec-prefix=[PATH]`         |                                                         |
| `--program-prefix`*                     | See below                                               |
| `--program-suffix`*                     | See below                                               |
| `--program-transform-name`*             | See below                                               |
| `--bindir=[PATH]`                       | `CMAKE_INSTALL_BINDIR=[PATH]`                           |
| `--libdir=[PATH]`                       | `CMAKE_INSTALL_LIBDIR=[PATH]`                           |
| `--sysconfdir=[PATH]`                   | `CMAKE_INSTALL_SYSCONFDIR=[PATH]`                       |
| `--includedir=[PATH]`                   | `CMAKE_INSTALL_INCLUDEDIR=[PATH]`                       |

> TODO:
> --with-x, --x-includes, --x-libraries,
> --build, --host, --target,
> --*-silent-rules, maintainer-mode,
> reconfigure
> --enable-doxygen-*
> --with-docker-*
> --winebottle


The following environment variables are used, if present:

* `$CC`, the C compiler to use
* `$CXX`, the C++ compiler to use
* `$FC`, the Fortran compiler to use
* `$CFLAGS`, flags appended to the compile commands for all C files
* `$CXXFLAGS`, flags appended to the compile commands for all C++ files
* `$FCFLAGS` or `$FFLAGS`, flags appended to the compile commands for all Fortran files
* `$PYTHON`, the python interpreter to use (and what `$PyLib` to use when running the tests)
* `$JAVA_HOME`, the default location to look for java tools (overridden by `JDK_DIR`)
* `$D3DLIB_PATH`, if `ENABLE_D3D=ON` this path will be searched for `libd3`
* `$HDF5_ROOT_DIR`, the path to search for the HDF5 libraries

**CMAKE_BUILD_TYPE**

There are actually four or more possible values:
* `Debug`, Build with debug symbols (roughly `-g` or equivalent)
* `Release`, Build with optimizations and `NDEBUG` (roughly `-O3 -DNDEBUG` or equivalent)
* `MinSizeRel`, Similar to `Release` but optimized for size (roughly `-Os -DNDEBUG` or equivalent)
* `RelWithDebInfo`, Similar to `Release` but with debug symbols (roughly `-O2 -g -NDEBUG` or equivalent)

Run `ccmake` or the CMake GUI for a full list of possible values for your system.

**CMAKE_INSTALL_PREFIX**

With the move to CMake, we no longer make a distinction between the `--prefix=PREFIX` and the `--exec-prefix=EPREFIX`. If this causes issues with your workflow, contact slwalsh@psfc.mit.edu.

**--jars** ???

**ENABLE_IDL** / **IDL_DIR**

If `ENABLE_IDL=ON` is specified, the standard installation directories for IDL will be searched.

If IDL is not installed in a standard directory, you can specify `IDL_DIR=[PATH]` to inform CMake of its location. Doing so will set `ENABLE_IDL=ON` automatically.

**--program-prefix** / **--program-suffix** / **--program-transform-name**

With the mover to CMake, we no longer support these options. If this causes issues with your workflow, contact slwalsh@psfc.mit.edu.

### Using alternative generators

CMake does not require you to use `make`, and in fact provides a long list of alternatives.
You can see what generators your CMake supports with:

```
cmake --help
```

Here are some examples:

**Ninja**

```
mkdir build
cd build
cmake -GNinja ..
ninja
```

**Visual Studio**

```
mkdir build
cd build
cmake -G"Visual Studio 17 2022" -Ax64 ..
# Open the generated .sln
```

**Xcode**

```
mkdir build
cd build
cmake -GXcode ..
# Open the generated Xcode project
```