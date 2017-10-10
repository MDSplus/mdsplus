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

    # cd mdsplus
    # ./configure
    # make all
---------------------------------------------------------------------------
Who Uses MDSplus
This map shows world fusion sites with MDSplus users marked in Blue.  To update
edit the file sites.csv and put in a pull request.

<p>
<iframe src="https://batchgeo.com/map/4cb145f00105157a242b5acafa8e646b" frameborder="0" width="100%" height="550" style="border:1px solid #aaa;"> </iframe>
</p>
<p>
<small>View <a href="https://batchgeo.com/map/4cb145f00105157a242b5acafa8e646b">MDSplus sites 2017</a> in a full screen map</small>
</p>

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
