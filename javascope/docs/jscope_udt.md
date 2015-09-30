
Using UDT in jScope                                               {#jscope_udt}
===================

In a network connection the MdsDataProvider is always interactively talking
with the server asking for a proper resampling of the real data to boundaries
and resolution suggested by the actual plot that has to be shown by scope in
that moment. This can dramatically reduce the download time required to render
the active plots. 

A test campaign proved that for a long distant connection charcterized by
hi-latency the UDT transfer protocol gain a throughput performance several
times higer then the TCP. The UDT protocol was integrated in jScope as a new
DataProvider. To activate this feature is only needed to select the
“MdsDataProviderUDT” server class in the network edit server list panel.

\image html img/dataprovider_UDT.jpg "UDT server selection"
\image latex img/dataprovider_UDT.jpg "UDT server selection" height=4cm


The UDT transport protocol is implemented in jScope using the \ref
jScope.MdsDataProviderUDT class. As before mentioned this object relies to the
low-level MDSplus libraries that are also used by the mdsip server. This
granted the full compliance of the client to the server any protocol language,
and in particular to the UDT choice at the cost of forcing the client to have
the target compiled MdsIp library for UDT connections. A sketch of the TCP and
UDT connection layers in a jScope user connection to a remote server is shown
in the following Figure.


\image html img/UDT_schema.jpg "TCP vs UDT connection path"
\image latex img/UDT_schema.jpg "TCP vs UDT connection path" height=4cm

The way we achieved this was using the Java Native Interface binding the data
transport routines of MdsIp library into the \ref jScope.MdsDataProvider. The
MdsDataProvider is implemented to use two internal streams for input and output
connected to java sockets. These streams provide the network transport layer
while MdsConnection class manages the Mdsip protocol to unwrap messages into
actual signals. So a new class called \ref jScope.MdsIpProtocolWrapper was
added to build new Input and Output streams (respectively MdsIpInputStream and
MdsIpOutputStream) for the connection using the underlying transport functions
provided by mdsip routines. In particular the functions used by the new streams
are declared in mdsip_connections.h as:

> 
> * int ConnectToMds(char connection_string); 
> * IoRoutines *GetConnectionIo(int id);
>

According to the Mdsip protocol plugin system the ConnectToMds is used to
create a Mdsip Connection structure, setting the client compression value and
performing the login session to the server. The GetConnectionIo gives direct
access to the transport plugin functions sending and receiving buffers (see
mdsip manual for details).


When the \ref jScope.MdsDataProviderUdt connects to server it makes an instance
of \ref jScope.MdsProtocolWrapper that, in turn, creates the new Mdsip io
streams. The ConnectToMds function is called by the constructor passing the
connection path that for the UDT case will simply prepend the “udt://” prefix.
Finally the new MdsIpInputStream redefines all the read() methods of
java.io.InputStream calling the JNI version of the ioRoutines receive, and
MdsIpOutputStream redefines all the write() methods of java.io.OutputStream to
call the JNI version of the ioRoutines send.
