MDSplus Components  {#mdsplus_components}
==================


The followings are the main MDSplus software components. They are build into
separate shared libraries for each host machine using the autotools build
system. This manual contains only a brief description of what the component
does as a separate documentation should be available.

### List of components:

 * \subpage lib_mdsshr

 * \subpage lib_treeshr

 * \subpage lib_tdishr

 * \subpage lib_mdsip

 * \subpage lib_mdsobjects


To have a simple localization of the software component within the MDSplus
source tree we shall refer to the following scheme:


\dot "MDSplus source directory structure for component localization" height=8cm
digraph example {
#    splines=ortho;
    ranksep=0.05;    

    node[shape=record, fontname=Serif, fontsize=9, color=lightgray, style=filled];
    mdsshr[label="mdsshr" URL="\ref lib_mdsshr"];
    treeshr[label="treeshr" URL="\ref lib_treeshr"];
    tdishr[label="tdishr" URL="\ref lib_tdishr"];
    mdstcpip[label="mdstcpip" URL="\ref lib_mdsip"];
    mdsobjects[label="mdsobjects" URL="\ref lib_mdsobjects"];
    mdsobjects_cpp[label="cpp"    URL="\ref mdsobjects_cpp_manual"];
    mdsobjects_py [label="python" URL="\ref mdsobjects_py_manual" ];
    mdsobjects_java [label="java" URL="\ref mdsobjects_java_manual" ];

    node[shape=none, color=none, style=solid];
    root[label="MDSplus source"];
    d_mdsshr[label="general support library"];
    d_treeshr[label="tree storage system"];
    d_tdishr[label="tdi language interpreter"];
    d_mdstcpip[label="remote data access"];
    d_mdsobjects_cpp[label="C++ objects interface"];
    d_mdsobjects_py [label="Python objects interface"];
    d_mdsobjects_java [label="Java objects interface"];
    

    node[label="", width=0, height=0];
    edge[arrowhead=none, color=blue];

    
    root -> n5;


    {rank=same; n5; mdsshr; d_mdsshr; }
    n5 -> mdsshr;
    n5 -> n1;
    
    {rank=same; n1; treeshr; d_treeshr;}
    n1 -> treeshr;    
    n1 -> n6;

    {rank=same; n6; tdishr; d_tdishr; }
    n6 -> tdishr;
    n6 -> n2;
    
    {rank=same; n2; mdstcpip; d_mdstcpip; }
    n2 -> mdstcpip;
    n2 -> n3;

    {rank=same; n3; mdsobjects;}
    n3 -> mdsobjects;
      mdsobjects -> n4;
      {rank=same; n4; mdsobjects_cpp; d_mdsobjects_cpp; }
      n4 -> mdsobjects_cpp;
      {rank=same; n7; mdsobjects_py; d_mdsobjects_py; }
      n7 -> mdsobjects_py;
      n4 -> n7;
      {rank=same; n8; mdsobjects_java; d_mdsobjects_java; }
      n8 -> mdsobjects_java;
      n7 -> n8;

}
\enddot



