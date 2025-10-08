![Build Status](https://jenkins.mdsplus.org/buildStatus/icon?job=MDSplus%2Fcmake)

## We want to know who you are
The MDSplus developers want to know who you are. If you or your site is using
MDSplus please fill out the following survey. We promise not to share your contact
information. Please do not assume that others from your institution have also filled
this out, we will combine the results. 

[MDSplus User Survey](https://docs.google.com/forms/d/e/1FAIpQLScSsA-fY2yTsW076bBreJmNbBqY9jsd-m4vmAdPvfCxXidiOQ/viewform?usp=sf_link).

## Installing from Pre-built Packages

We highly recommend you install MDSplus from our pre-built packages, available
on [mdsplus.org](https://mdsplus.org/index.php?title=Downloads&open=110608406848219381791&page=Software%2FDownloads).
However, if you need to customize the build you can follow the instructions below.

## Building MDSplus from Source

To build and install MDSplus, you will need to obtain the
MDSplus distribution from GitHub either as a Git repository or as a 
compressed tar file.

### Cloning/Downloading

To clone the repositiory:
```sh
git clone https://github.com/MDSplus/mdsplus.git
```

This will create a directory called `mdsplus/` in your current working directory.

To download the tarball:
```sh
wget https://github.com/MDSplus/mdsplus/tarball/alpha -O MDSplus.tgz
```

And then untar it:
```sh
tar -xzf MDSplus.tgz
cd MDSplus-mdsplus-<TAB>
```

However, without Git, the build system cannot determine version information.
You will need to specify the release tag in order to build with branch/version
information. Add `-DRELEASE_TAG=alpha_release-1-2-3` (with the correct branch
and version number, separated by dashes) to your `cmake-arguments` below.

### Building without Docker

When building without Docker, all required libraries, tools, and packages will
need to be installed on your system. To build without installing dependencies,
see the [Building with Docker](#building-with-docker) section below.

**To build the software using our automated tooling:**

```sh
./deploy/build.py -j [<arguments>] [<cmake-arguments>]
```

This will place build files in `./workspace/build/`. You can then enter an interactive
environment to test the build with:

```sh
# This will "install" mdsplus into ./workspace/install/usr/local/mdsplus/
./deploy/build.py --install
./deploy/build.py -i
source setup.sh
```

**Or to manually build the software do the following:**

```sh
mkdir build
cd build
cmake <mdsplus-src-dir> [<cmake-arguments>]
make -j
```

**Or follow the legacy build instructions:**

```sh
mkdir build
cd build
<mdsplus-src-dir>/bootstrap
<mdsplus-src-dir>/configure [<arguments>]
make -j
```

These are no longer officially supported, but will output the equivalent CMake
command given a set of original `./configure` arguments, and can be helpful
while migrating. See the [CMake Migration Guide](CMakeMigrationGuide.md) for
more information.

### Building with Docker

If you are building for Linux or Windows, you can build with Docker.
When building with Docker, you don't need to install any special libraries or
tools, however you will need `docker` installed and permission to run it on your system.

First, you must select an OS to build. For a full list, run:

```sh
./deploy/build.py --help
```

and check the bottom of the output. You need to pick an OS as similar to your
target system as possible, so that library versions are compatible. For this
example we will be using Ubuntu 24.04 (amd64), which can be specified with
`--os=ubuntu-24-amd64`.

Now, run `build.py` with your chosen OS:

```sh
./deploy/build.py -j --os=ubuntu-24-amd64 [<arguments>] [<cmake-arguments>]
```

This will place build files in `./workspace-ubuntu-24-amd64/build/` (or
`workspace-` followed by whatever you specified for `--os`). As mentioned
above, you can enter an interactive environment to test the build with:

```sh
# This will "install" mdsplus into ./workspace-{OS}/install/usr/local/mdsplus
./deploy/build.py --os=ubuntu-24-amd64 --install
./deploy/build.py --os=ubuntu-24-amd64 -i
source setup.sh
```

Then, to generate installation packages, use:

```sh
./deploy/build.py --os=ubuntu-24-amd64 --package
```

and tarball files will be placed in `./workspace-{OS}/packages/`, and
installation kits (`.deb`, `.rpm`, etc) will be placed in `./workspace-{OS}/dist/`

For a listing of additional `cmake-arguments`, please see the [CMake Migration Guide](CMakeMigrationGuide.md).

---------------------------------------------------------------------------

# Who Uses MDSplus
This map shows world fusion sites using MDSplus.  
(Use the \<back\> button in the browser to return to this page)

https://drive.google.com/open?id=1Lt3r3dnAtx79anaLarJkKb4l0s5RWpPn&usp=sharing

---------------------------------------------------------------------------
# Special Notes

LessTif conflicts:

1) If the LessTif package is installed then configure will find it before it
finds the real Motif package. LessTif is a marginal implementation of Motif
and the MDSplus X applications will not build or run using LessTif package.

2) To build on a system with LessTif installed, add `-DMotif_ROOT=/path/to/motif/`
to your `cmake-arguments` 

Sybase Open/CLient - IDL database connection

Sites with IDL, Sybase Open/CLient libraries and Microsoft SQL Server
databases can use `idlsql` library for database connectivity.  The IDL
procedure in `idl/Logbook/dbinfo.pro` should be edited to reflect the
site specific database connectivity and security policies.  The default
version assumes that the environment variable `$SYBASE_HOST` contains
the name of the host from the Sybase interfaces file.  It looks for a
file `$HOME/xxx.sybase_login`, where xxx is the name of the Sybase host,
if it finds it and can read it, it sends the first line as the username
and the second for the password, if not it sends $USER and a default
password.

Note: To inform CMake of a non-standard installation directory for Sybase,
add `-DSybase_ROOT=/path/to/sybase` to `cmake-arguments`.
