Object oriented bindings {#bindings}
------------------------

A thin client connection implementation example is described in the following.
The thin client connection of mdsobjects relies on the Connection interface
that exposes the following methods:

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
 The connection constructor creates the socket using ConnectToMds().
 This has the effect to instance a connection structure inside the available 
 connections list and performs the necessary login actions and compression settings.
 All further operations on the server are executed via TDI expressions.
 
 ### openTree
 openTree() calls MdsOpen() function that executes the TDI "OpenTree($,$)" with the
 tree name and shot args. The tree opened session is held by the server process.
 
 ### closeTree and closeAllTree
 The former coloses the remote opened tree inside server via the TDI "CloseTree()" expression
 evaluation. The latter execute the same on all opened trees on the server side.

 ### setDefault
 Sets the base node for relative path computation. The TDI "TreeSetDefault($)" expression
 is executed in MdsSetDefault().
 
 ### get and put
 The get and put functions has the purpouse of performing generic expression evaluation and
 sending the result through the connection. If the client calls a get of a TDI command, the
 expression is sent to the server and so remote evaluated. The result is therefore transmitted 
 back to the client. On the contrary the put function asks the server to execute the "TreePut($,$)" 
 command to store the values passed as arguments.

