
# Developing MDSplus with CMake

## Adding a Library

1. Create a new directory (if needed)

2. Create a CMakeLists.txt (if needed)
   Multiple libraries/executables can be defined in the same CMakeLists.txt

3. Add the following to compile and link your new library, make sure to
   replace the source files with your own.
   Note: All libraries will be linked as shared unless otherwise specified 
   if `BUILD_SHARED_LIBS=ON`, which is the default.

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

4. Linking other libraries not only adds `-lLibrary` to the linker command,
   but also carries with it any `PUBLIC` settings the library had. This
   includes include directories, compiler options, defines, and whatever
   else is needed to use that library.

   All MDSplus libraries can be linked just by their name, and external 
   libraries can be linked using their interface library name. For the
   dependencies that are built-in to CMake, you can find extensive documentation
   on CMake's website, including what names to use here. For custom dependencies,
   look in `cmake/Find*.cmake` to see what is available.

   See the `Dependencies` section of the root `CMakeLists.txt` for what libraries
   are available.

```cmake
target_link_libraries(
    ExampleShr
    PUBLIC
        TreeShr
        Threads::Threads
)
```

5. Configuring other options such as include directories or compiler options and
   defines can be done with several functions in the form `target_*`. For more
   information, see the CMake documentation for each function.

```cmake
target_include_directories(
    ExampleShr
    PUBLIC
        path/to/include
)

target_compile_definitions(
    ExampleShr
    PRIVATE
        -DVERSION=1.2.3
)

target_compile_options(
    ExampleShr
    PRIVATE
        -Wno-specific-error
)
```

6. If you want both a static and dynamic version of your library, you need to define
   a separate library with a different name all the same options. This is a bit
   cumbersome, so we added a function to help named `mdsplus_add_static_copy()`.
   If `BUILD_SHARED_LIBS=OFF`, then our library would already be static, and this
   function call becomes a noop.

```cmake
mdsplus_add_static_copy(ExampleShr _static_target)

# ${_static_target} will be the name of the static library if it was made, or ""
```

7. Installing a library is done by marking the targets for installation. If you 
   used `mdsplus_add_static_copy()`, then you will want to add `${_static_target}`
   to this call as well.

```cmake
install(TARGETS ExampleShr ${_static_target})
```

## Adding an Executable

## Adding a Java JAR

## Adding a Test

## Adding a Dependency

## Visual Studio Code