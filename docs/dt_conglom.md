Conglomerate (aka Device) {#dt_conglom}
=========================

MDSplus provides an conglomerate data type used for describing data acquisition
or analysis devices. A device in MDSplus is implemented as a collection of
adjacent tree nodes (or a conglomerate). The first node (located at the top of
the node structure of this collection) contains a conglomerate data type. The
conglomerate data type is a structure containing an image part, a model part, a
name part and a qualifiers part. MDSplus implements device support by providing
a mechanism for performing "methods" on devices. When a device method is
performed on a device in an MDSplus tree either through an action or using the
TCL command DO/METHOD, MDSplus invokes a procedure determined by the
information in the head node of the device which contains a conglomerate data
type. If there is an image part of the conglomerate structure, MDSplus will
attempt to call a routine called "model-part"__"method" in the shared library
specified by the image part. If the routine can not be found, MDSplus will try
to invoke the TDI function called "model-part"__"method". Normally, the
conglomerate data is loaded into the head node of a device automatically when
you add it to the tree so you may not need to access this data type directly.

The name part of the device is often a reference to a sub node of the device
which contains the data acquisition module identification such as the CAMAC
module name. This portion of the device is not used during method invokation
and was simply added as a convenience in building tools for gathering
information about device definitions in a tree. The qualifiers part of the
structure could be used by device support for making variant device
implementations. This field along with the name field are generally not used
any longer.

The following shows an example of a conglomerate data type.

    TCL> set tree subtree
    TCL> dir/full a14

    \SUBTREE::TOP

     :A14
          Status: on,parent is on, usage device,readonly
          Data inserted:  3-NOV-1994 14:43:55    Owner: [100,100]
          Dtype: DTYPE_CONGLOM         Class: CLASS_R             Length: 102 bytes
          Model element: 1

     Total of 1 node.

     TCL> decompile a14
     Build_Conglom("MIT$DEVICES", "A14", *, *)

     TCL> do/method a14 store

In the above example we open the subtree (sample tree) and display the
characteristics and contents of the A14 node. It contains a DTYPE_CONGLOM data
record and is the first element of a conglomeration of nodes. The conglomerate
data structure has "MIT$DEVICES" for the image part and "A14" for the model
part and the name and qualifiers parts are empty. When the store method is
performed on this node (or any node which is part of this device), MDSplus
looks for a routine called a14__store in the shared library called MIT$DEVICES
and if found calls it with the node number of the head node and the method
string. If it cannot find this routine or image, it will attempt to invoke the
a14__store tdi function with the same arguments.

The following table lists some of the TDI functions available for creating and
accessing conglomerate data types:

\latexonly { \tiny \endlatexonly

| **Function**   | **Description**                              |
|----------------|----------------------------------------------|
| BUILD\_CONGLOM | Build a conglomerate structure               |
| IMAGE\_OF      | Return the image part of a conglomerate      |
| MAKE\_CONGLOM  | Make a conglomerate structure                |
| MODEL\_OF      | Return the model part of a conglomerate      |
| NAME\_OF       | Return the name part of a conglomerate       |
| QUALIFIERS\_OF | Return the qualifiers part of a conglomerate |

\latexonly } \endlatexonly
