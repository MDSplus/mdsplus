Generate doxygen documentation  {#use_docs}
==============================


This explains how to make MDSplus self generating documentation.

MDSplus is now using \em Doxygen (http://www.doxygen.org) to automatically build 
code documentation for several components. 
The build process of Doxygen has been also enhanced using autotools scripts. 
The overall organization of documents follows the path hierarchy
of the code itself, so if we can think MDSplus components as separate libraries
each of these libraries will have its own documentation directory and its own 
doxygen Makefile. As a general policy we choosed to name this directory "docs" 
but it is not mandatory.

This subdirectory has the role of insulating the actual code distribution to the 
docs generation, indeed it has a separate selfconsistent Makefile.am script where
all documented pages have to be declared toghether with the documentation options.
It also may contins also any plain text file witten in \em markdown language 
(http://en.wikipedia.org/wiki/Markdown) that is parsed by doxygen to add text 
only pages to the documents.

A special purpouse directory was also defined at the MDSplus top source directory,
here all the doxygen related files, such as configurations and layout template, are
stored. This directory represents also the main documentation folder where links
of all the sublibraries are indexed.




How does it work
-----------------

A doxygen related autoconf script has been added to the framework:

    m4
    `-- ax_prog_doxygen.m4
    
This is the main script that aclocal includes to handle doxygen generation. It 
defines a function that is called from autoconf script configure.ac:

    DX_INIT_DOXYGEN(mdsplus, doxygen.cfg)
    DX_HTML_FEATURE(ON)
    DX_PDF_FEATURE(ON)
    
As you can see inside the m4 and in all automake script files there are several 
variables defined to set doxygen options, each declared with a DX_xxx name
(to remind the user that it is doxygen related). Exporting these variables to the
Makefile target script environment doxygen can set its own config options inside
the config file when executed.

The followings are the special files that documentation relies on. They can be 
found in your MDSplus source code "docs" directory.

    docs
    |-- Makefile.am
    |-- config
    |   |-- doxygen.am
    |   |-- doxygen.cfg
    |   |-- doxygen_cpp.cfg
    |   `-- style
    |       |-- footer.html
    |       |-- header.html
    |       |-- institutes.png
    |       |-- logo.jpg
    |       `-- mds_style.css
    
Here the config directory holds doxygen related files. The most important are 
"doxygen.am" and "doxygen*.cfg" that are where the scripts are mainly defined.
A short step by step guide to include a target follows:

Adding doxygen handled sublibrary
---------------------------------

We shall see how a "sublib" library would have been included in the MDSplus docs.
The main steps to be implemented to define a new documentation library are three:



### 1) Create docs Makefile 

First create "docs" directory and Makefile.am file inside that has to include the
automake doxygen script \em ${top_srcdir}/docs/config/doxygen.am

    sublib
    |-- docs
    |   `-- Makefile.am

Write in that file the following settings:

    SUBDIRS = .
    include ../../docs/config/doxygen.am
    
    # DOXYGEN CONFIG ################################################
    DX_CONFIG = "${top_srcdir}/docs/config/doxygen.cfg"
    DX_DOCDIR = sublib/docs
    DX_PACKAGE_NAME = sublib
    DX_TITLE = MDSsublib
    DX_BRIEF = "MDSplus example sublibrary"
    
    DX_PAGES = .. \
           index.md \
           other_page.md
                      
    DX_EXCLUDE_PAGES = \
                   excluded_from_doxy.h \
                   other_excluded.c \
                   
    DX_TAGFILES = docs.tag
    MOSTLYCLEANFILES = $(DX_CLEANFILES)

    # TARGETS ########################################################
    all: html    
    html: doxygen-run
    pdf:  doxygen-pdf
    install-data-local: doxygen-install


All documents are generated from the code found in DX_PAGES list, we assumed 
here that the actual code was in the parent directory "docs/.." and two further
\em .md pages of documentation were defined.

The index.md file is mandatory and represents the \em MainPage section of the 
documentation. Here we use to define an *anchor* to this manual that can be 
referenced within other libraries. The name this anchor should be uniform with
\em "name"_manual pattern. So index of "sublib" will be:

    anchor sublib_manual
    
    Indroduction
    ------------
    sublib is a sub library of MDSplus. MDSplus is a set of software tools for 
    data acquisition and storage and a methodology for management of complex 
    scientific data.
    MDSplus allows all data from an experiment ... bla bla ...
    
    .. you can ref to other pages or code symbols with \ref command    
    .. or can ref to other manuals or pages with \subpage command
    
    
    a link to the main documentation page can be added in this way:
    
    \note For a general description of the overall MDSplus framework please refer
          to \subpage mdsplus_manual "MDSplus manual"




### 2) Add tag files

The way doxygen can link among different libraries is the tag description.
For a reference of doxygen Tags and exterlan linkage see http://www.doxygen.org

Tags are usefull as they can keep references of the code symbols among the
built libraries. \image html img/search_tips.png


Each time MDSplus build a documentation it also build a special target called
"build_tag" that write tag information relative to current project inside the
/docs/dtags special directory. The tag filename is the name of the library plus
".tag" extension. In addition the generation scripts also adds a line, containing 
the tagfile name and the docs location, in the "taglist.txt" file.

Once the code has been generated 

Now all other libraries that wants to include external symbols must define in
the variable DX_TAGFILES a "grep" string pattern that uniquely identifies the
taglist.txt entry.

In the "sublib" example above the docs.tag entry identifies the docs main manual
form the tag name.


### 3) Link library from MDSplus main manual

The main docs manual has a special Makefile.am script that has to account the 
new library. Let us open the ${top_sourcedir}/docs/Makefile.am and see how 
sublib would be added:

    SUBDIRS = \
          ../mdstcpip/docs/ \
          ../mdsobjects/cpp/docs/ \
          ../sublib/docs \
          .
    # ^^^^^^^^ sublib directory added here
    
    DX_PAGES = index.md \
               lib_mdsip.md \
               lib_mdsobjects.md \
               lib_sublib.md \
               use_docs.md
    # ^^^^^^^^ adds lib_sublib.md to describe sublib library in main documentation
    
    DX_TAGFILES = mdsip.tag \
                  mdsobjects/cpp/docs
                  sublib.tag
    # ^^^^^^^^ adds sublib.tag file so main docs can refs to new library symbols
    
In this way the main documentation can brawse all sub libraries symbols and can
access pages and anchors .. usually in the main index.md there is a link to the
sublibrary MainPage ( \\subpage sublib_manual in this case ).





Launch the document generation
------------------------------

So here we go, we have the new library ready and we can launch the generation of
the code. This is done using make all target in the docs directory

    make -C sublib/docs html
    make -C sublib/docs pdf
        
It generates the tags and the html documentation you will find in sublib/docs/html/index.html

If you want to build all MDSplus documentation you can run make from main docs 
directory:

    make -C docs 
    
and to have it installed to the prefix target dir
   
    make -C docs install


Prevent Doxygen intrusion
-------------------------

Sometime the excessive doxygen detail on code leads to an unreadable documentation.
To prevent it reading a selection of the code a special define was implemented.
Here is an example:


    #include <iostream>

    /// This code will be documented
    int docfunc() {}
           
    #ifndef DOXYGEN // hide this part from documentation    
    int nodoc() {
      // this function will NOT be part of documentation
    }
    #endif // DOXYGEN end of hidden code





\note 
 Hope you enjoyed this way of documenting the code ... to submit an issue or to 
 have any further detail feel free to contact me andrea.rigoni@igi.cnr.it













