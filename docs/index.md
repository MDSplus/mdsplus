
\anchor mdsplus_manual

\attention 
The MDSplus source documentation is in beta stage, for a complete general
description of MDSplus please refer to the [Mdsplus
site](http://www.mdsplus.org).

\htmlonly 
<blockquote class="doxtable">
<p>A pdf version of this manual is available here: <a href="../docs.pdf">MDSplus manual</a></p>
</blockquote> 
\endhtmlonly


Introduction
============

Welcome to MDSplus software manual. MDSplus is a set of software tools for data
acquisition and storage and a methodology for management of complex scientific
data. MDSplus allows all data from an experiment or simulation code to be
stored into a single, self-descriptive, hierarchical structure. The system was
designed to enable users to easily construct complete and coherent data sets.
The MDSplus programming interface contains only a few basic commands,
simplifyng data access even into complex structures. Using the client/server
model, data at remote sites can be read or written without file transfers.
MDSplus includes x-windows and java tools for viewing data or for modifying or
viewing the underlying structures. Developed jointly by the Massachusetts
Institute of Technology, the Fusion Research Group in Padua, Italy (Istituto
Gas Ionizzati and Consorzio RFX), and the Los Alamos National Lab, MDSplus is
the most widely used system for data management in the magnetic fusion energy
program. It is currently installed at over 30 sites spread over 4 continents.
The MDSplus developers hope that MDSplus may prove to be a valuable tool for
research at your site.


Index of MDSplus manuals
------------------------

This is the main MDSplus manual, it aims to provide an guide to the MDSplus
software components. The following is a table of the currently documented
components, please note that some manuals are yet in development stage so the
overall software coverage is actually limited. To get a more complete reference
of the MDSplus tools and of the components not shown here please refer to the
[Mdsplus site](http://www.mdsplus.org).


| Brief description          |  Reference manual                         |
|:---------------------------|-------------------------------------------|
| \ref lib_mdsshr            | \subpage mdsshr_manual                    |
| \ref lib_tdishr            |     not written yet, ref to Mdsplus site  |
| \ref lib_treeshr           |     not written yet, ref to Mdsplus site  |
| \ref lib_mdsip             | \subpage mdsip_manual                     |
| \ref lib_mdsobjects C++    | \subpage mdsobjects_cpp_manual            |
| \ref lib_mdsobjects Python | \subpage mdsobjects_py_manual             |
| \ref lib_mdsobjects Java   | \subpage mdsobjects_java_manual           |
| MDSlib manual              | \subpage mdslib_manual                    |
| Java Scope signal viewer   | \subpage javascope_manual                 |


<!-- ---------------------------------- -->
<!-- This keeps the related pages order -->
<!-- ---------------------------------- -->

\page mdsplus_components MDSplus Components
\page data_types Data Types
\page use_docs Appendix A - generate documentation


   

