
# Developing MDSplus with CMake

## Quick Reference

```sh
# Simply build
./deploy/build.py -j

# Build and run all tests on a debug build
./deploy/build.py -j --test -DCMAKE_BUILD_TYPE=Debug

# Build and package a release build for Ubuntu 24.04 amd64
./deploy/build.py -j --package -DCMAKE_BUILD_TYPE=Release --os=ubuntu-24-amd64

# Run an interactive build prompt, and configure the environment for testing
./deploy/build.py -j --install
./deploy/build.py -i
source setup.sh

# See a list of available options for --os
./deploy/build.py --help
# (at the bottom of the output)
```

## Building

Building MDSplus can either be done manually with `cmake`, or by using `build.py`.

### Building Manually

```sh
mkdir build
cd build/
cmake .. <cmake-arguments>
make -j
```

There are also many tools (such as Visual Studio Code with the CMake Tools 
extension) that will detect a `CMakeLists.txt` and automatically run similar 
steps.

For a full list of `cmake-arguments` see `cmake --help` and do the following:

```sh
cd build/

# Open the CMake GUI
cmake-gui

# Or open the CMake TUI to allow you to edit options
ccmake ..

# Or just print the options and current values
cmake -LH ..
```

### Building with `build.py`

**Native**
```sh
./deploy/build.py -j <arguments> <cmake-arguments>
```

This will build MDSplus into `workspace/build/`.

**For a given `OS`**
```
./deploy/build.py -j --os=OS <arguments> <cmake-arguments>
```

This will build MDSplus into `workspace-OS/build/` where `OS` is the value 
passed to `--os`.

For a full list of `arguments` for `build.py`, as well as a full list of 
potential values for `--os`, run `./deploy/build.py --help`.

## Testing

### Using `ctest`

CMake comes with a test-running tool called `ctest`, which can be used to run 
any/all of our tests.

```sh
cd build/
# or cd workspace/build/

# Run all tests
ctest

# Run all tests in parallel
ctest -j

# Run with verbosity (or with -VV for extra verbosity)
ctest -V

# Run all tests matching '*ExampleTest*'
ctest -R 'ExampleTest'

# Run all tests in the mdsobjects/ folder
ctest -R '^mdsobjects'

# Only run tests that failed last run
ctest --rerun-failed
```

**Note:** If you want to use `ctest` to run tests inside of a docker container, use 
`build.py` in interactive mode, then follow the steps above.

### Using `build.py`

You can also use `build.py` to run our tests by passing `--test`. Some options 
from `ctest` are also available, such as `-R` and `--rerun-failed`. We wrap 
`ctest` for several reasons:
  * It allows us to easily build and test with one command
  * It allows Jenkins to easily run the tests and collect a jUnit XML file with the results
  * It summarizes the results of testing, and prints log filenames for easy access  
  (**Note:** They can be Ctrl+Clicked in Visual Studio Code to open them directly)

```sh
# Build and run all tests
./deploy/build.py --test

# Build and run all tests in parallel
./deploy/build.py --test -j

# Run all tests matching '*ExampleTest*'
./deploy/build.py --test -R 'ExampleTest'

# Run all tests in the mdsobjects/ folder
./deploy/build.py --test -R '^mdsobjects'

# Build and run only tests that failed last run
./deploy/build.py --test --rerun-failed
```

## Debugging

### Debugging with Visual Studio Code

This is the recommended way to debug. Please see 
[Visual Studio Code Integration](#visual-studio-code-integration) below.

### Debugging with `gdb`

The easiest way to debug with `gdb` directly is using `build.py` in interactive 
mode.

```sh
# This will install MDSplus into workspace/install/usr/local/mdsplus
./deploy/build.py --install <arguments>

# This will enter the interactive build prompt
# **Note:** If you are using a --os with a Docker Image, this prompt will be inside the Docker Container
./deploy/build.py -i <arguments>

# This is a helper script that sources the setup.sh in workspace/install/usr/local/mdsplus
source setup.sh

# You can now run or debug any MDSplus executables
```

In order to rebuild and debug the new executables/libraries, you can either exit 
the interactive prompt, or use the helper scripts in the workspace.

```sh
./do-build.sh
./do-install.sh
```

## Packaging

To generate packages, use `build.py` with the `--package` option. 
This also requires several other options such as `--arch` and `--distname`, so 
it is best run with a `--os` that is already configured for packaging.

This will generate packages/installers for the given distribution: `.deb` files 
for Debian variants, `.rpm` files for Red Hat variants, and `.exe` for Windows. 
Additionally, this will generate `.tgz` files of `workspace/install/usr/local/mdsplus` 
and of any packages/installers that were generated.

```sh
./deploy/build.py --os=ubuntu-24 --package

# Installers will be placed here:
ls workspace-ubuntu-24-amd64/dist

# Tarfiles will be placed here:
ls workspace-ubuntu-24-amd64/packages
```

## Adding an Option

There are many options for controlling the build available through CMake. These 
allow you to turn on/off features, indicate where to look for dependencies, or 
activate tooling such as `valgrind` or the sanitizers.

To add an option, add a call to `mdsplus_option()` in the `Options` section 
towards the top of the root [CMakeLists.txt](CMakeLists.txt). 
When possible, try to group similar options together. CMake does provide an 
`option()` function, however this has many limitations that are overcome
by using `mdsplus_option()`. For additional information see 
[./cmake/MDSplusOption.cmake](./cmake/MDSplusOption.cmake) or look at the calls 
to it in the root [CMakeLists.txt](CMakeLists.txt).

```cmake
# Define a boolean option
mdsplus_option(
    ENABLE_MY_FEATURE BOOL
    "Enable my feature"
    DEFAULT OFF
)

# Define a string option
mdsplus_option(
    THING_API_VERSION STRING
    "The API version to use for libThing, in the format of MAJOR.MINOR.PATCH"
    DEFAULT "1.2.3"
)

# Define an option only when not on Windows
if(NOT WIN32)
    mdsplus_option(
        USE_COOL_LINUX_FEATURE BOOL
        "Use a cool feature only available on linux"
        DEFAULT ON
    )
endif()

# Define an option with a complicated default
set(_use_workaround_default OFF)
if(APPLE)
    set(_use_workaround_default ON)
endif()

mdsplus_option(
    USE_WORKAROUND BOOL
    "Use a workaround"
    DEFAULT ${_use_workaround_default}
)
```

These options will all be available when using the CMake GUI (`cmake-gui`) or
TUI (`ccmake`), or when listing cache variables with `cmake`. `mdsplus_option()`
will also include the default at the end of the description, so make sure not 
to repeat it.

```sh
cmake-gui
ccmake
cmake -LH ..
```

Finally, all options registered with `mdsplus_option()` will be shown during 
configure so that you always have a snapshot of the build configuration in the 
same output as the build. This is especially helpful when looking at Jenkins 
builds.

## Adding a Library

1. Create a new directory (if needed)

2. Create a `CMakeLists.txt` (if needed)  
   **Note:** Multiple libraries/executables can be defined in the same `CMakeLists.txt`.

3. Add a call to `add_subdirectory()` in the `Libraries, Executables` section 
   of the root [CMakeLists.txt](CMakeLists.txt) (if needed).

4. Add the following to compile and link your new library, making sure to
   replace the source files with your own.  
   **Note:** All libraries will be linked as shared unless otherwise specified if 
   `BUILD_SHARED_LIBS=ON`, which is the default.

    ```cmake
    ###
    ### ExampleShr
    ###

    add_library(
        ExampleShr
        source1.c
        source2.cpp
        source3.f
    )
    ```

5. Linking other libraries not only adds `-lLibrary` to the linker command, but 
   also carries with it any `PUBLIC` settings the library had. This includes 
   compiler options, include directories, defines, and whatever else is needed 
   to use that library.

   All MDSplus libraries can be linked just by their name, and external 
   libraries can be linked using their interface library name. For the 
   dependencies that are built into CMake, you can find extensive 
   documentation on CMake's website, including what names to use here.
   
   See the `Dependencies` section of the root [CMakeLists.txt](CMakeLists.txt), 
   or in `cmake/Find*.cmake` for what libraries are available.

    ```cmake
    target_link_libraries(
        ExampleShr
        PUBLIC
            TreeShr
            Threads::Threads
    )
    ```

6. Configuring other options such as include directories or compiler options 
   and defines can be done with several functions in the form `target_*()`. For 
   more information, see the CMake documentation for each function.  
   **Note:** The include directories are marked `PUBLIC`, meaning that other 
   targets that link against this library will be able to find our header files.

    ```cmake
    target_include_directories(
        ExampleShr
        PUBLIC
            path/to/include # Relative to the current directory
    )

    target_compile_definitions(
        ExampleShr
        PRIVATE
            API_VERSION=1.2.3
    )

    target_compile_options(
        ExampleShr
        PRIVATE
            -Wno-specific-error
    )
    ```

7. If you want both a static and shared version of your library, you need to 
   define a separate library with a different name but with all the same 
   options. To help with this, we added `mdsplus_add_static_copy()`.
   If `BUILD_SHARED_LIBS=OFF`, then our library would already be static, and 
   this function call becomes a no-op.

    ```cmake
    mdsplus_add_static_copy(ExampleShr _static_target)

    # ${_static_target} will be the name of the static library if it was made, or ""
    ```

8. Installing a library is done by marking the targets for installation. If you 
   used `mdsplus_add_static_copy()`, then you will want to add `${_static_target}`
   to this call as well.

    ```cmake
    install(TARGETS ExampleShr ${_static_target})
    ```

## Adding an Executable

1. Create a new directory (if needed)

2. Create a `CMakeLists.txt` (if needed)  
   **Note:** Multiple libraries/executables can be defined in the same `CMakeLists.txt`.

3. Add a call to `add_subdirectory()` in the `Libraries, Executables` section 
   of the root [CMakeLists.txt](CMakeLists.txt) (if needed).

4. Add the following to compile and link your new executable, making sure to
   replace the source files with your own.  

    ```cmake
    ###
    ### example
    ###

    add_executable(
        example
        source1.c
        source2.cpp
        source3.f
    )
    ```

5. Configuring other options such as include directories or compiler options and
   defines can be done with several functions in the form `target_*()`. For more
   information, see the CMake documentation for each function.  
   **Note:** Unlike with a library, all options should me marked as `PRIVATE` as 
   there is no good reason to link an executable as if it were a library.

    ```cmake
    target_include_directories(
        example
        PRIVATE
            path/to/include # Relative to the current directory
    )

    target_compile_definitions(
        example
        PRIVATE
            API_VERSION=1.2.3
    )

    target_compile_options(
        example
        PRIVATE
            -Wno-specific-error
    )
    ```

## Adding a Java JAR

1. Create a directory for the Java project (if needed).

2. Create a `CMakeLists.txt` (if needed). It is recommended to only include one 
   JAR per directory.

3. Add a call to `add_subdirectory()` in [java/CMakeLists.txt](java/CMakeLists.txt)
   (if needed). This will properly disable your JAR when `ENABLE_JAVA=OFF`, and 
   will ensure it is managed with the other Java projects.

4. Create a `MANIFEST.MF` file, usually by generating it through CMake.

    ```cmake
    set(_manifest ${CMAKE_CURRENT_BINARY_DIR}/MANIFEST.MF)

    file(WRITE ${_manifest}
        "Specification-Version: ${RELEASE_VERSION}\n"
        "Implementation-Version: ${RELEASE_VERSION}\n"
        "Implementation-Vendor-Id: org.mdsplus\n"
    )
    ```

5. Collect all sources and resources, usually done with `file(GLOB_RECURSE)`.

    ```cmake
    file(GLOB_RECURSE
        _source_list
        "src/main/java/*.java"
    )

    file(GLOB_RECURSE
        _example_resource_list
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "src/main/resources/example/*"
    )
    ```

6. Call `add_jar()` to register the JAR for building.

    ```cmake
    add_jar(
        jExample
        SOURCES ${_source_list}
        # If there are no resources, you can skip this
        # If there are multiple namespaces, you can list them under RESOURCES
        RESOURCES
            NAMESPACE "example" ${_example_resource_list}
        ENTRY_POINT mds.example.jExample
        MANIFEST ${_manifest}
        # If other MDSplus JAR files needed to be included:
        # INCLUDE_JARS mdsobjects
    )
    ```

7. Call `install_jar()` to register the JAR for installation.

    ```cmake
    install_jar(
        jExample
        DESTINATION java/classes
    )
    ```

8. Some Java tools come with scripts to easily run them, see the `Scripts` 
section of [java/jtraverser/CMakeLists.txt](java/jtraverser/CMakeLists.txt) for 
how to include them.

## Adding a Test

1. Create a `testing/` subdirectory in the tool/library you want to test (if needed).

2. Create a `CMakeLists.txt` in this directory to configure the tests (if needed).

3. Add a call to `add_subdirectory()` in the `Testing` section near the bottom
   of the root [CMakeLists.txt](CMakeLists.txt) (if needed).

4. Create a loop to configure the tests (if needed). You can look at
   [treeshr/testing/CMakeLists.txt](treeshr/testing/CMakeLists.txt) or
   [mdsobjects/cpp/testing/CMakeLists.txt](mdsobjects/cpp/testing/CMakeLists.txt) 
   for reference. For most libraries, this loop already exists and you can simply 
   add your test source to the list. The important pieces are:

  * A list of test sources, each of which will be compiled into an executable 
    with the same name.
  * A loop over this source list.
  * A call to `cmake_path()` to get the `STEM` of a source name, which is the 
    name of a file without the directory or file extension.
    For example, `MyTest.c` -> `MyTest`.
  * A call to `add_executable()` to register the test executable, which allows 
    us to reference it in future CMake calls, specifically `mdsplus_add_test()`. 
    If you have additional sources that need to be compiled into each 
    executable, such as a file containing utility functions, you can add 
    additional sources to the call to `add_executable()`.
  * A call to `target_link_libraries()` that links the executable against the 
    library you are intending to test. Most tests will want to link against 
    `MdsTestShr`, which contains our implementation of `check`. If you are trying 
    to test something that isn't a library, you can skip this step.
  * A call to `mdsplus_add_test()`, which is our wrapper around CMake's
    `add_test()`. This is what configures all of the settings for our test. There 
    are several options available, and the documentation for it lives in 
    [cmake/MDSplusAddTest.cmake](cmake/MDSplusAddTest.cmake).  
    **Note:** `$<TARGET_FILE:${_name}>` is a CMake generator expression, which is 
    something that will evaluate later than other CMake code, allowing us to get 
    the exact filename of a given executable. See the 
    [CMake Documentation](https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html) 
    for more information.

5. Add your test source code, and make sure it is in the list of test sources. The 
   most important thing is the return code: if the test returns 0 then it is logged 
   as a success, and anything else is logged as failure. The logs from the test, 
   along with the environment variables it was run with, will all be logged in 
   Jenkins.  
   **Note:** If you are using `MdsTestShr`, there are a few key elements to 
   include, such as:

    * `#include <testing.h>`.
    * A normal `int main(int argc, char * argv[])` function.
    * `BEGIN_TESTING(NAME)` and `END_TESTING`, with `NAME` being replaced by your 
      test name. These allow multiple tests to be run in a single executable, and 
      enforce the test timeout configured in `check`, the default is 20 minutes.
    * `TEST1(EXPR)` and `TEST0(EXPR)` check that the expression equals 1 or 0, 
      respectively.
    * `TEST_ASSERT(EXPR)` is an alias for `TEST1`.
    * `TEST_TIMEOUT(TIMEOUT)` will change the test timeout in `check`, this will be 
      removed when moving to `gtest` as CMake now controls the test timeout.
    * **Note:** `SKIP_TEST` and `ABORT_TEST` no longer function as intended and 
      will be removed; currently they appear as test failures.

### FAQ

**What if I want to use a tree?**

The `$default_tree_path` will be set to the test's `WORKING_DIRECTORY`, and the 
default for that is `CMAKE_CURRENT_BINARY_DIR`. This is the "equivalent" directory 
in the build tree to the current directory, For example, the current binary directory 
for `treeshr/testing/` would be `build/treeshr/testing`. This ensures all test 
artifacts get cleaned up when removing the build directory.

The best way to include a tree is to open it for `NEW` and build the tree as part 
of the test. However, you do have access to the `main` and `subtree` trees that are 
included in the `trees/` directory at the root of the repository. It is only 
recommended to use these read-only, as you could compromise other tests. Be careful 
of other tests in the same directory, as tree names and shot numbers between them 
can conflict.

**What if I want to use a port?**

Ports are regulated by [testing/ports.csv](testing/ports.csv) in order to keep 
tests isolated when running in parallel. Find the next unused port/range in 
`ports.csv` and mark it as in-use by your test.

**What if my test breaks in `valgrind`?**

You can skip `valgrind` for a given test by passing `NO_VALGRIND` to `mdsplus_add_test()`.

**What if my test breaks in `wine`?**

You can skip a test when running with `wine` by passing `NO_WINE` to `mdsplus_add_test()`.

**What if my test needs additional environment variables?**

You can pass additional environment variables to `mdsplus_add_test()` using 
`ENVIRONMENT_MODIFICATIONS`. See the 
[CMake Documentation](https://cmake.org/cmake/help/latest/prop_test/ENVIRONMENT_MODIFICATION.html) for more information.

## Adding a Dependency

Some libraries and tools are findable using built-in CMake scripts. When adding a 
dependency, first check CMake's documentation to see if it is already available. We 
use many examples that fit this description, such as:

* [Python](https://cmake.org/cmake/help/latest/module/FindPython.html)
* [Java](https://cmake.org/cmake/help/latest/module/FindJava.html)
* [BLAS](https://cmake.org/cmake/help/latest/module/FindBLAS.html)
* [LibLZMA](https://cmake.org/cmake/help/latest/module/FindLibLZMA.html)

Other dependencies must be found manually, the preferred method for doing this is 
with a CMake find script. This is a script named `FindNAME.cmake` where `NAME` is 
the name of your library, that lives on the `CMAKE_MODULE_PATH`. We keep ours in 
[cmake/](cmake/) and have a standard format for their contents. When adding a new 
dependency this way, copy an existing script and modify it for your purposes. There 
are many examples of dependencies that we've added this way, such as:

* [LibFFI](./cmake/FindLibFFI.cmake)
* [Motif](./cmake/FindMotif.cmake)
* [Sybase](./cmake/FindSybase.cmake)
* [Readline](./cmake/FindReadline.cmake)
* [Globus](./cmake/FindGlobus.cmake)

Regardless of how the library is found, the actual call to find it is stored in the 
root [CMakeLists](CMakeLists.txt) in the `Dependencies` section. Most are marked 
as `REQUIRED`, as the build will not succeed without them.

```cmake
# cmake/FindMyLib.cmake
find_package(MyLib REQUIRED)
```

All "find" scripts will set CMake variables with their results, such as 
`MYLIB_INCLUDE_DIRS` and `MYLIB_LIBRARIES` for our example above. These can be used 
directly, but it is preferable to use an interface target if available. These are 
"fake" libraries that carry with them all of the configuration needed to use an 
external library, as if it were a target built by our project. For our example, it 
would probably look like `MyLib::MyLib`. These can then be used with 
`target_link_libraries()` like so:

```cmake
target_link_libraries(
    ExampleShr
    PUBLIC
        MyLib::MyLib
)
```

**Note:** The redundancy in the interface library names may seem strange, but think 
of the first half as a namespace. Many dependencies have multiple targets in their 
namespace, such as `Python::Python` and `Python::Interpreter`.

## Visual Studio Code Integration

There is now tooling to help develop/debug MDSplus with Visual Studio Code, 
available through `build.py`.

First, make sure you have the [clangd](vscode:extension/llvm-vs-code-extensions.vscode-clangd) 
extension installed.

Then, run `build.py` as you do normally, but with the `--setup-vscode` flag. This 
should only be done once. If you are using a `--os` argument, make sure to keep it, 
but note that debugging won't work unless your system is compatible with the 
binaries built in Docker. This will configure [.vscode/settings.json](.vscode/settings.json) 
for syntax highlighting and code completion, and [.vscode/launch.json](.vscode/launch.json) 
for launching tests for debugging.

```sh
./deploy/build.py -j --setup-vscode
```

Make sure you follow the instructions at the end of the output, notably running 
"clangd: Restart language server".

To debug with VS Code, go to the `Run and Debug` tab on the left, select the test 
from the list at the top, and then click the `Start Debugging` button (the green 
play icon). In addition to debugging tests, there are also several utility targets 
available at the bottom of the list, including:

* `mdsip-8888`, which runs `mdsip` on port 8888
* `tdic`
* `tditest`
* `python`, which runs the same python version used during the build with `$PYTHONPATH` set
* `mdstcl`

Note, all of these will run with the "test environment", which is to say that they 
will all point at the build folder for MDSplus libraries and executables, and will 
have a custom `$default_tree_path` set. You can open [.vscode/launch.json](.vscode/launch.json) 
and edit the settings for easier debugging, but they will be overwritten the next 
time you run `build.py --setup-vscode`.