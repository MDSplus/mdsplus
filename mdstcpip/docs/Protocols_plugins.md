Protocol plugins {#mdsip_plugins}
----------------

When a user connects to a remote server using MDSplus MDSip, the connection string
specified will determine the protocol to use for the connection.
When the connection is first made, a shared library with the name
MdsIp"protocol-name-in-caps" is loaded which will provide the routines necessary
to preform the actual transfer of bytes to and from the remote server.
For example the TCP transport protocol plugin is implemented in the library named
`MdsIpTCP.so` (or `MdsIpTCP.dll` for Windows users).


Support for a given protocol includes the implementation of up to 9 routines to
provide the following low level I/O operations:

    Io - return structure containing the addresses of the following support routines
    Connect - Connect to a remote server
    Send - Send data buffer to a remote server
    Recv - Receive a data buffer from the remote server
    Flush - Flush any remaining bytes in the communication buffers
    ReuseCheck - Provide a string representation of a proposed connection (explained below)
    Disconnect - Disconnect from remote server
    Listen - Server side support. Listen for connections from remote clients
    Authorize - Server side support. Authenticate client connection




The routines anchor function
----------------------------

To implement a protocol plugin you should implement the functions above.
The shared library needs to export only one entry point, the "Io" function.

    IoRoutines *Io()

This routine take no arguments and simply returns a pointer to a \ref
_io_routines structure defined in the \ref mdsip_connections.h include file.
This structure contains pointers to the I/O functions used to implement the
protocol.

    typedef struct _io_routines {
      int (*connect) (int conid, char *protocol, char *connectString);
      ssize_t (*send)(int conid, const void *buffer, size_t buflen, int nowait);
      ssize_t (*recv)(int conid, void *buffer, size_t len);
      int (*flush) (int conid);
      int (*listen) ( int argc, char **argv);
      int (*authorize) (int conid, char *username);
      int (*reuseCheck) (char *connectString, char *uniqueString, size_t buflen);
      int (*disconnect) (int conid);
    } IoRoutines;

Usually the `Io` function returns a static precompiled instance of IoRoutines
defined inside library.








List of plugins already implemented
-----------------------------------

 | Plugin    | Description                               |  Implementation    | Example address            |
 |:----------|:------------------------------------------|:-------------------|:---------------------------|
 | **TCP**   | IPv4 Transport Protocol                   | IoRoutinesTcp.c    | tcp://hostname[:port]      |
 | **TCPV6** | IPv6 Transport Protocol                   | IoRoutinesTcpv6.c  | tcpv6://hostname[:port]    |
 | **UDT**   | IPV4 Reliable UDP communicaton layer      | IoRoutinesUdt.c    | udt://hostname[:port]      |
 | **UDTV6** | IPv6 Reliable UDP communicaton layer      | IoRoutinesUdtV6.c  | udtv6://hostname[:port]    |
 | **GSI**   | Auth with Globus Security Infrastructure  | IoRoutinesGsi.c    | gsi://hostname[:port]      |
 | **SSH**   | TCP connection in a SSH tunnel            | IoRoutinesTunnel.c | ssh://[username@]hostname  |
 | **HTTP**  | TCP connection in a HTTP tunnel           | IoRoutinesTunnel.c | http://[username@]hostname |


