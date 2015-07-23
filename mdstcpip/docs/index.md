\anchor mdsip_manual

>
> This is the user manual of a specific MDSplus software component, For a
> general description of the overall MDSplus framework please refer to:
>
>  * \ref mdsplus_manual "The Reference manual"
>  * [The MDSplus site](http://www.mdsplus.org/index.php)
>

\htmlonly 
<blockquote class="doxtable">
<p>A pdf version of this manual is available here: <a href="../mdsip.pdf">MDSip manual</a></p>
</blockquote> 
\endhtmlonly


MDSip (MDSplus remote data acess protocol)
==========================================

MDSplus provides a very simple but powerful protocol for connecting to remote
servers to perform tasks such as retrieving or writing data and performing actions.
In an MDSip session, the client program opens a connection to a remote server and
the communicates with that server by sending an MDSplus TDI expression text string
along with optional parameters which are scalars or arrays of text or numbers.
The server process evaluates the expression and sends the result as a scalar or
array of text or numbers. Except for a few other low level functions used by
MDSplus internals, that is the main functionality of MDSip. Although the concept
is simple, combining this functionality with the capabilities of TDI enables MDSip
to become a gateway for data exchange even if the data is not stored in native
MDSplus format. The original implementation of MDSip was done in 1994 to give
Unix based system access to MDSplus data which at the time only existed on
OpenVMS servers. It has become a very important capability of MDSplus ever since,
providing access to data and providing an infrastructure for distributed computing
for data acquisition and analysis.



Transport "Plugins"
-------------------

Attempts to provide additional authentication options and to add features such as
parallel streams for increased throughput of MDSip connections over the years caused
the low level MDSip code to be cluttered with conditional compilations to enable
different versions of the code to be built with a wider variety of transport options.
In 2010, a redesign of the MDSip code was performed to separate the MDSip message
processing from the underlying transport layer. In 2011, the MDSip client/server
software was rewritten based on this new design. With this new implementation, new
low level transports can be deployed without any modifications to the underlying
MDSip code. The choice of transport can now be specified in the connection string
passed to the MdsConnect routine which now has a format similar to a http
url:protocol:://protocol-specific-connection-string.
If old style connection strings are used (i.e. hostname[:port] or _hostname[:port])
they default to tcp and gsi transports respectively. In the initial implementation
the following transports were supported:

  * tcp://hostname[:port] - equivalent to the original hostname[:port] connection
  * gsi://hostname[:port] - Globus Security Infrastructure, equivalent to _hostname[:port]
  * udt://hostname[:port] - Uses UDT transport protocolo (see http://udt.sourceforge.net)
  * ssh://[username@]hostname - Uses ssh authentication to connect to remote server.
  * http://hostname[:port]/MDSplusWsgi/mdsip - Uses http to connect to remote server






\page mdsip_plugins  Protocol plugins
\page mdsip_clients  Clients connection
\page mdsip_bindings Objects oriented bindings
