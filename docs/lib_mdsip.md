"mdstcpip" The remote data acess {#lib_mdsip}
================================

\image html img/planet_mdsplus.png "MDSplus is broadly adopted framework in the fusion community"
\image latex img/planet_mdsplus.png "MDSplus is broadly adopted framework in the fusion community" height=6cm
 

MDSplus provides a very simple but powerful protocol for connecting to remote 
servers to perform tasks such as retrieving or writing data and performing 
actions. In an MDSip session, the client program opens a connection to a remote 
server and the communicates with that server by sending an MDSplus TDI 
expression text string along with optional parameters which are scalars or 
arrays of text or numbers. The server process evaluates the expression and 
sends the result as a scalar or array of text or numbers. Except for a few 
other low level functions used by MDSplus internals, that is the main 
functionality of MDSip. Although the concept is simple, combining this 
functionality with the capabilities of TDI enables MDSip to become a gateway 
for data exchange even if the data is not stored in native MDSplus format. 
The original implementation of MDSip was done in 1994 to give Unix based system 
access to MDSplus data which at the time only existed on OpenVMS servers. 
It has become a very important capability of MDSplus ever since, providing 
access to data and providing an infrastructure for distributed computing for 
data acquisition and analysis.

For further details please refer to the MdsIp component manual

|   GO TO THE COMPONENT MANUAL  |
|-------------------------------|
| \ref mdsip_manual             |

