/*  VAX/DEC CMS REPLACEMENT HISTORY, Element NODETOUCH.H */
/*  *3     8-MAR-1993 11:41:42 JAS "can't use string rename in enum" */
/*  *2    18-MAY-1992 11:56:19 JAS "add some more operations" */
/*  *1    14-MAY-1992 14:52:26 JAS "Include file for Node touch types" */
/*  VAX/DEC CMS REPLACEMENT HISTORY, Element NODETOUCH.H */
/*
   C include file for NodeTouch types.
   Defines the ways in which a utilitiy (TCL) 
   can affect a node.
*/
typedef enum {on_off, rename_node, delete, new, tree, set_def} NodeTouchType; 
