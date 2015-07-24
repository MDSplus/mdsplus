Object oriented bindings {#mdsip_bindings}
------------------------

C++ Thin Client
---------------

The thin client connection of C++ interface in mdsobjects relies in the
MDSplus::Connection class that exposes the following methods:

    Connection(char *mdsipAddr);                                            
    void openTree(char *tree, int shot);                                    
    void closeTree(char *tree, int shot);                                    
    void closeAllTrees();                                                    
    void setDefault(char *path);                                            
    Data *get(const char *expr, Data **args, int nArgs);                    
    Data *get(const char *expr);                                            
    void put(const char *path, char *expr, Data **args, int nArgs);         
    PutMany *putMany();                                                     
    GetMany *getMany();                                                     



 ### Constructor
 
 The connection constructor creates the socket using \ref ConnectToMds(). This
 has the effect to instatiate a connection structure inside the available
 connections list and performs the necessary login actions and compression
 settings. All further operations on the server are executed via TDI
 expressions.
 
 ### openTree 
 
 openTree() calls \ref MdsOpen() function that executes the TDI "OpenTree($,$)"
 with the tree name and shot args. The tree opened session is held within the
 server connection context.
 
 ### closeTree and closeAllTree
 
 Both colose the remote opened tree and subtrees inside server calling
 MdsClose().

 ### setDefault
 
 Sets the base node for relative path computation. The TDI "TreeSetDefault($)"
 expression is executed in MdsSetDefault().
 
 ### get and put
 
 The get and put functions has the purpouse of performing generic expression
 evaluation and sending the result through the connection. If the client calls
 a get of a TDI command, the expression is sent to the server and so remote
 evaluated. The result is therefore transmitted back to the client. On the
 contrary the put function asks the server to execute the "TreePut($,$)"
 command to store the values passed as arguments.



 ### Transfering a Signal Segment
 
 The write of a signal segment into a remote tree is a typical example of data 
 communication that can be handled by the Thin Client.
 Moreover this appears to be the best choise to handle the large signal tranfer 
 among distant end points.
 The way this transfer is accomplished using TC access method is a special TDI
 command executed to the server. We call get() method passing the `MakeSegment()` 
 command that is valuated within the server context returning to the client only
 the result status. The actual data transfer resides in the passage of the command
 arguments. 
 
 The MakeSegment() function is declared as in the following:
 
     public fun MakeSegment(as_is _node, 
                            in _start, 
                            in _end, 
                            as_is _dim, 
                            in _array, 
                            optional _idx, 
                            in _rows_filled)
    
 where \em _node is the tree node target path, \em _start and \em _end are the 
 start and end times to localize the segment inside the signal, \em _dim is the
 signal dimension, \em _array is the data content of the signal segment, _idx is
 an optional identifier associated to the segment and finally \em _rows_filled is
 the number of ...

 The get() implementation in very similar to the MDSip function MDSValue() so the
 sequence of descriptors that we can expect to be transferred are the command string
 followed by the 7 over declared arguments.
 An example of a easy one-dimensional signal segment transfer can be the following:
 
     connection.get("MakeSegment($1,$2,$3,make_range($2,$3,$4),$5,,$6)",args,6);
 
 where the actual creation of the dim array is demanded to the server with the 
 make_range() nested call.
 So the descriptors passed in this case are in order: 
 
 * the TDI string command "MakeSegment($1,$2,$3,make_range($2,$3,$4),$5,,$6)"
 * the string representing the path of the node in target tree,
 * the start, the end and the sample time,
 * the data array and the array size.
 
 All these descriptors are converted to MDSip messages filling the \ref Message 
 structure and iteratively sent to the server.
 A final message that carries the result of function evaluation is sent back to 
 the client.
 
 
 
 
