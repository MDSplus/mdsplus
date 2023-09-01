![Build Status](https://jenkins.mdsplus.org/buildStatus/icon?job=MDSplus%2Fcmake)

## We want to know who you are
The MDSplus developers want to know who you are. If you or your site is using MDSplus please fill the following survey. We promise not to share your contact information. Do not assume that other's from your institution have also filled this out, we will combine the results. 

[MDSplus User Survey](https://docs.google.com/forms/d/e/1FAIpQLScSsA-fY2yTsW076bBreJmNbBqY9jsd-m4vmAdPvfCxXidiOQ/viewform?usp=sf_link).

## Building MDSplus

### Building from source manually
To build and install MDSplus on unix systems, you will need to obtain the
MDSplus distribution from github either as a git repository or as a 
compressed tar file.

To clone the repositiory:

  git clone https://github.com/MDSplus/mdsplus.git

This will create a directory mdsplus under your current
default.

To download the tarball
  wget https://github.com/MDSplus/mdsplus/tarball/alpha 

And then untar it (The exact filename will depend on the release version):

  tar -xzf MDSplus-mdsplus-alpha_release-6-3-423-5-gca7f90d.tar.gz

To build the software do the following (See special notes below):

    # mkdir build
    # cd build
    # <mdsplus-src-dir>/configure
    # make

However this requires having all the needed libraries and packages installed
and there are many different options to configure for building MDSplus.
When MDSplus is built to generate releases for public distribution a deploy
script is used which uses Docker to pull specially configured Docker images
from https://hub.docker.com/r/mdsplus/docker/

### Building using Dockerized build environment
You can build MDSplus for any of the linux and windows operatings systems
without needing to install any special compilers or libraries. If you have
the mdsplus source code and Docker installed on your linux system and your
account has privileges to run docker then you can build MDSplus using
a simple command:

    # cd build
    # <mdsplus-src-dir>/deploy/build.sh --os=fc25 --branch=<branch> \
    --release=<version i.e. 7.99.100>
    
The above command will build the installer packages using the sources in
`<mdsplus-src-dir>/...` for the operating system selected. The packages will
be named based on the branch and version specified. Note the branch option
is only used for naming the packages. It does not checkout or create a git
branch. There are many more options to the build.sh script which you can
see by typing:

    # <mdsplus-src-dir>/deploy/build --help
    
You can find the available operating systems that you could specify for the
--os=<operating-system> by doing a directory as follows:

    # ls <mdsplus-src-dir>/deploy/os/*.opts

    
---------------------------------------------------------------------------
Who Uses MDSplus
This map shows world fusion sites using MDSplus.  
(Use the <back> button in the browser to return to this page)

https://drive.google.com/open?id=1Lt3r3dnAtx79anaLarJkKb4l0s5RWpPn&usp=sharing

---------------------------------------------------------------------------
Special Notes

For D3D sites that want to build the MDSplus/d3d ptdata interface you must
first define an envionment variable D3DLIB_PATH set to the directory
containing the d3 access library, libd3. Then invoke configure as follows:

./configure --enable-d3d

You can build the Motif applications using the static libraries so that
the binaries can be used on systems where Motif is not installed. To enable
the use of static Motif libraries use the --enable-staticmotif flag:

./configure --enable-staticmotif

Certain optional packages installed on system may be incompatible with some
or all of the MDSplus package. We will try to keep a list of potential
problems found in this document along with possible workarounds.

F77 compiler problems:

If configure cannot locate an appropriate F77 compiler it will abort. If you
have an F77 compiler then define the environment variable F77 to the binary
for the compiler. If you don't have an F77 compiler, define environment
variable NOF77 to yes and it will allow the configure to continue. You can
build most of MDSplus without an f77 compiler.

LessTif conflicts:

1) If the LessTif package is installed then configure will find it before it
finds the real Motif package. LessTif is a marginal implementation of Motif
and the MDSplus X applications will not build or run using LessTif package.

To build on a system with LessTif installed do the following:


setenv LD_LIBRARY_PATH [motif-lib-directory]\:$LD_LIBRARY_PATH
setenv UILPATH [motif-bin-directory-where-uil-image-is]
./configure --x-libraries=[motif-library-directory]  \
            --x-includes=[motif-include-directory]

replacing the text inside the brackets (and the brackets) with the
location of the real Motif files.

SYBASE Open/CLient - IDL database connection
Sites with IDL, SYBASE Open/CLient libraries and Microsoft SQLSERVER
databases can use idlsql library for database connectivity.  The idl
procedure in idl/Logbook/dbinfo.pro should be edited to reflect the
site specific database connectivity and security policies.  The default
version assumes that the environment variable SYBASE_HOST contains
the name of the host from the sybase interfaces file.  It looks for a
file $HOME/xxx.sybase_login, where xxx is the name of the sybase host,
if it finds it and can read it, it sends the first line as the username
and the second for the password, if not it sends $USER and a default
password.
NOTE - the environment variable SYBASE must point at the sybase distribution
in order for their libraries to function.
NOTE - if the sybase distribution is not /usr/local/sybase, configure
will not find the distribution unless you first set the environment
variable SYBASE to point the the distribution directory.

This extra line added to force a build
