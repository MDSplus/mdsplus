Clients connection {#clients}
==================

Several data access libraries are available in MDSplus. 
In particular using the C interface the "mdslib" library is intended to provide 
local data access while the remote access is provided by this library.

The different access methods to access data stored in MDSplus trees can be divided
in this categories:

  - Local
  - Thin client
  - Thick client
  - Distributed

The "Local" method is used when no explicit connection to an MDSplus server is
performed and the tree path definition refers to local file systems.
All expressions are evaluated in you process context and the files are accessed
using direct file I/O. The MDSip library in not involved in this kind of access.

The "thin client" method is used when you do a connection to an MDSplus data server
directly. In the mdsobject cpp interface this is done using the Connectin object.
All expression evaluation and file I/O is done by the server.
No tree path definitions are required on the client machine.
This method places most of the cpu and I/O load on the server machine.
Since the data is decompressed and evaluated on the server before sending the
results to the client, this method usually results in a heavy load on the network.

The "thick client" method is used when no explicit connection to an MDSplus data
server is made and the tree path definition is in the form "host::".
In this access mode, all expression evaluation is performed on the client but
the tree file I/O is done by the server. The server uses its local tree path
definition for finding the trees and the server uses tree access routines to
retrieve data from the trees and return it to the client.

The "distributed" mode of access is used when no explicit connection to an
MDSplus data server is made and the tree path definition is of the form:
"host::dir-spec". In this mode, all expression evaluation and data decompression
is done by the client. The server performs efficient low-level file I/O on
behalf of the client. This mode fully supports "distributed" trees in that
subtrees can reside on different hosts.


Thin client
===========

This is the simplest form of the remote interface to a MDSplus data server. The 
connection and the transfer of data is indeed fully demanded to the user.
The core of the connection is managed by very few commands, leaving all actual
higher level operations to the passage of TDI commands and decriptors agruments.

The steps for reading/writing in a remote MDSplus trees are:

 ||
 |:------------------------------------------------|
 | 1. Establish a connection with a mdsip server   |
 | 2. Open a tree                                  |
 | 3. Read and write data                          |
 | 4. Close the tree                               |
 | 5. Disconnect from the mdsip server             |

This is actually the same procedure of the local mdslib access to a tree where the
step 1 and 5 has been added to manage the remote connection.


As a tree node may contain data of an arbitrary type (string, scalar, array, integer, float etc.), 
we need a mechanism for mapping the returned data type into the receiving variable. 
In the low-level data management libraries of MDSplus a descriptors infrastructure 
was adopted, originally coming from the OpenVMS system.


In particular, a descriptor will specify:
 * The type of the receiving variable
 * Its dimension
 * Its starting address 

So these descriptors simply contain a reference to the actual instance
and the total length of memory occupied. This is strongly architecture dependent
and for this reason MDSip always checks that each data transfer is done matching the 
same memory organization of the server.

The client memory organization is always recorded inside client_type field of the
\ref Message passing through the connection, the actual value is then tested to match
the client type using ClientType() function.
As the tree data stored to the server is organized in its own memory architecture
the client must convert each descriptor trasaction.
The conversion of the message is handled using the FlipHeader() and FlipData() 
functions each time a client starts to get or put data to the server.


The main functions used within a mdsip session are provided by mdsip_connections.h  
where the core of mdsip functionalities are declared. They are:


- NewConnection() Creates the new connection structure and appends it to the client
                  connection list. This also loads the proper protocols routines finding
                  the protocol library requested.

- ConnectToMds()  This function creates a \ref Connection structure, sets the clinet
                  compression value and perfoms the login session to the server.
                 
- SendMdsMsg()    This is the main function that actually sends all data through
                  the connection protocol using send routine of the loaded protocol.

- GetMdsMsg()     This is the conunterpart of the latter holding the process while a message
                  is not received from the connection using the recv routine of the loaded
                  protocol.

- SendArg()       The remote expression evaluations usually needs some arguments to be
                  tranferred through the channel. SendArgs is a wrapper around the SendMdsMsg
                  function to handle this arguments passing. A \ref Message structure is built
                  carring in the argument as a descriptor.

- GetAnswerInfo() and GetAnswerInfoTS() wait for the server message response of an expression
                  evaluation.

- MdsValue()      This is the remoted version of the usual mdslib function to execute a TDI
                  expression. The remote execution exit status code is retured back to client.


 
The simplest way to transfer data through the connection is to send to create a 




 
 













