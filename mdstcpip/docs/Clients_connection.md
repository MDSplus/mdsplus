Clients connection {#mdsip_clients}
==================

Several data access libraries are available in MDSplus. 
In particular, using the C interface, the "mdslib" library is intended to provide 
local data access while this library provides the remote access.

The different methods to access data stored in MDSplus trees can be divided
in this categories:

  - Local
  - Thin client
  - Thick client
  - Distributed

The "Local" method is used when no explicit connection to an MDSplus server is
performed and the tree path definition refers to local file systems.
All expressions are evaluated within the same process context and the files are accessed
using direct file I/O. The MDSip library in not involved in this kind of access so 
it will not be described in the folloiwng ( see treeshr for further details ).

The "thin client" method is used when you do a connection to an MDSplus data server
directly.
All expression evaluations and file I/O is done by the server.
No tree path definitions are required in the client machine environment variables.
This method places most of the cpu and I/O load on the server machine.
Since the data is decompressed and evaluated within the server before sending the
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

 |                                                 |
 |:------------------------------------------------|
 | 1. Establish a connection with a mdsip server   |
 | 2. Open a tree                                  |
 | 3. Read and write data                          |
 | 4. Close the tree                               |
 | 5. Disconnect from the mdsip server             |

This is actually the same procedure of the local mdslib access to a tree where the
step 1 and 5 has been added to manage the remote connection.


As a tree node may contain data of an arbitrary type (string, scalar, array, integer, 
float etc.), 
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

The client memory organization is always recorded inside the client_type field of the
\ref Message structures passing through the connection, this value is then tested to match
the client type using \ref ClientType() function.
As the tree data stored to the server is organized in its own memory architecture
the client must convert each descriptor trasaction.
The conversion of the message is handled using the \ref FlipHeader() and \ref FlipData() 
functions each time a client starts to get or put data to the server.


The main functions used within a mdsip session are provided by mdsip_connections.h  
where the core of mdsip functionalities are declared. They are:


- \ref NewConnection() Creates the new connection structure and appends it to the client
                       connection list. This also loads the proper protocols routines finding
                       the protocol library requested.

- \ref ConnectToMds()  This function creates a \ref Connection structure, sets the clinet
                       compression value and perfoms the login session to the server.
                 
- \ref SendMdsMsg()    This is the main function that actually sends all data through
                       the connection protocol using send routine of the loaded protocol.

- \ref GetMdsMsg()     This is the conunterpart of the latter holding the process while a message
                       is not received from the connection using the recv routine of the loaded
                       protocol.

- \ref SendArg()       The remote expression evaluations usually needs some arguments to be
                       tranferred through the channel. SendArgs is a wrapper around the SendMdsMsg
                       function to handle this arguments passing. A \ref Message structure is built
                       carring in the argument as a descriptor.

- \ref GetAnswerInfo() and GetAnswerInfoTS() wait for the server message response of an expression
                       evaluation.

- \ref MdsValue()      This is the remoted version of the usual mdslib function to execute a TDI
                       expression. The remote execution exit status code is retured back to client.

 

Detailed description of the five steps of a thin client connection follows with 
particular focus on the use of the mentioned MDSip functions.
     
 ### 1) Establish a connection with a mdsip server
 
 The client that wants to connect to a mdsip server running on a remote machine
 has first to establish a valid connection through a Login process.
 The connection handshake for this procedure is shown in the picture below:
  
 
 \image html  img/mdsip_login.png "Mdsip login message sequence"
 \image latex  img/mdsip_login.pdf "Mdsip login message sequence"
 

 The link is requested using \ref ConnectToMds() function passing the remote server 
 address formatted as in protocol description (see \ref mdsip_plugins).
 This function creates a new \ref Connection instance at the server side, filled
 with parameters that it has beed parsed by address string and calling 
 \ref NewConnection() with selected protocol name.
 
 At the server side a listening socket has been initialized by the running mdsip 
 process, with proper protocol and port. The way that the actual connection is 
 catched is handled by the listen() IoRoutine method so it depends from the protocol
 that is going to be used. Anyway two options for handling incoming connections are 
 possible: the \em single and the \em multi clients connection.
 The single mode ("-s" server argument) makes the server accepting only one client 
 at a time granting full priority to that connection, this is particularly suitable
 for dispatching action messages. In this case a single active Connction structure
 is accounted by the server.
 In the multi mode ("-m" argument) connection many clients can be connected to a
 mdsip server handling the transfer process in a active socket queue. In this mode
 each connected client has been associated to a proper Connection pushed in the 
 connection list (see \ref FindConnection()).
 Please note that the multi mode server is not actually handling the opened sockets
 in parallel, its role is simply to switch the tree context matching the 
 active client. For a true parallel handling see the xinetd resident initialization.
 
 To establish the channel the client calls connect() member of its IoRoutines that
 triggers the connection handshake for the selected plugin. This is represented 
 by the arrow named "C" in the picture. Actually a bidirectional transfer is done
 at this point. For example in TCP we expect that the messages passed will be
 the usual three way handshake connection packets, while UDT will use an euristic 
 connection handling, attempting to reduce the connection time in high latency 
 connections.
 
 Once the soket has been opened the server enters the \ref AcceptConnection() procedure
 that actually instances the client \ref Connection structure. Then it puts itself
 in a loop waiting for data, calling \ref GetMdsMsg() function that triggers recv() 
 IoRoutine method. 
 The client steps into the \ref DoLogin() function that sends a \ref Message (label M1
 in figure) to the server holding the username string and the client compression 
 level in the status field.
 User is authorised at the server side using authorize() plugin routine, and the
 answer collected by \ref AcceptConnection() is sent back to the client by a new Message
 structure only filled in the status field.
 Here the status has a special formatting that carries the actual authorization 
 result in the first bit and the server value of accepted connection compression
 in the higher four bits.
 
 \note Please note that at the time of writing tha actual compression of the 
 established connection is always the level requested by client clipped inside
 an interval of [0, 32]. So the client is the only responsible in deciding how
 the compression level of the channel will be set.
 
 
 ### 2) Open a tree
 
 As the connection has been established, and all structures valorized with proper
 settings, the listen() routine enters a loop that asks the socket for new incoming 
 messages. When a new message arrives the funcion \ref DoMessage() handles it.
 The messages attended by the server are of two kinds: TDI expressions that have 
 to be evaluated, and special io commands identified by MDS_IO_xxxx macros inside 
 the code. The former is the method of almost all thin client transactions, the 
 latter is a set of tools that are used within the distributed client access and
 will be presented later.
 
 The tree opening comes through the internal funcion \ref MdsOpen() that recursively 
 executes the TDI function `Open($,$)` via MdsValue() call at the client side.
 
 \image html img/tc_open.png "Mdsip open remote tree sequence"
 \image latex img/tc_open.pdf "Mdsip open remote tree sequence"
 

 All the low level communication that unregoes the TDI remote evaluation is provided
 by the \ref SendArg() and \ref GetAnswerInfo() functions for the client.
 On the server side \ref DoMessage() calls an intenal library function called \ref ProcessMessage(),
 this collects the function string and all the arguments needed, converts values
 to the server memory mapping and finally calls the tdishr execution (see \ref ExecuteMessage()).
 
 
 
 ### 3) Read and write data
 
 All read and write data operations are done using the same pattern as the one
 shown in open tree picture. For better clarity the MdsValue function scheme is
 reported below.
 
 \image html img/tc_value.png "Mdsip remote MdsValue() sequence"
 \image latex img/tc_value.pdf "Mdsip remote MdsValue() sequence"
 
 The use of MdsValue is actually not mandatory, it symply iterates the SendArg()
 function to send the command string at the fisrt message and all agruments on 
 the others, then it waits for the server to respond with the command execution 
 exit status.
 Each message that is passing through has its own index to be always uniquely 
 identified.
 


 ### 4) Close tree
 
 As in the MdsOpen the Close action is sent by the client via the \ref MdsValue() function
 executing the `TreeClose()` TDI command. The tree path is not specified in the args
 as it has already been set within the active context in the opening procedure.

 ### 5) Close connection
 
 Finally, the client asks to close the channel calling disconnect() IoRoutine function.
 The socket descriptor is released and the connection shut down on both ends without
 any further message passed through the channel. All the arguments descriptors allocated
 inside the connection buffer is released also.
 




 


