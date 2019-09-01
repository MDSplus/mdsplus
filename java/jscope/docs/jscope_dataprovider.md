The data provider mechanism {#jscope_dataprovider}
===========================


jScope was designed to be able to display data in different formats trough the
definition of a specific reader object that is called the “Data Provider”.

The principle extensively adopted in the jScope interface is that the input
entities that the user provides are completely opaque statements that are
parsed within the specific handler that matches the chosen data source. From
the code point of view a general declaration for each data sources handler is
defined in DataProvider interface, and each way the application access data is
also a particular implementation of this class. It defines a set of methods
providing the signals that jScope will deal with and supporting several
features which may be enabled or disabled depending on the current
implementation choice. Moreover all signals are not returned by DataProvider
access methods as vectors (as it was done in earlier jScope versions), rather
an implementation of “WaveData” interface is always returned.


The DataProvider interface
--------------------------

>
> For a detailed description of the DataProvider interface see \ref
> jScope.DataProvider
>

The public methods declared by \ref jScope.DataProvider interface are listed
below:

\code {.java}
    void SetEnvironment (String exp) throws IOException
    void Update (String exp, long s)
    String GetString (String in) throws IOException
    double GetFloat (String in) throws IOException
    WaveData GetWaveData (String in)
    WaveData GetWaveData (String in_y, String in_x)
    void AddUpdateEventListener (UpdateEventListener l, String event) throws IOException
    void RemoveUpdateEventListener (UpdateEventListener l, String event) throws IOException
    FrameData GetFrameData (String in_frame, String in_times, float start_time, float end_time) throws IOException
    void AddConnectionListener (ConnectionListener l)
    void RemoveConnectionListener (ConnectionListener l)
    void SetArgument (String arg) throws IOException
    long[] GetShots (String in) throws IOException
\endcode


Each time we ask for a new waveform to display we have always to query the
DataProvider for data. First the scope instances the server creating a new
instance from introspecting all possible implementation of the interface. When
the selected implementation has been built it calls SetArguments and
SetEnviroment to pass the instance parameters and environment variables that
characterize the plot. Then we do the “apply” step to actually refresh the
plots. The scope first calls the Update method to set the correct tree and shot
in the active DataProvider instance, then it takes all active waveform paths
and the plots draw axes to trigger the GetWaveData or GetFrameData methods. As
before stated all returned objects are derived from WaveData interface and the
implementation of WaveData that we get form DataProviders are usually inner
classes of the provider self so they can manage also the asynchronous retrieval
of the remote data. Moreover the WaveData is also able to manage the signal
segmentation retrieving and collating segments into a composed wave form
representation. Many operations of the DataProvider can be asynchronous, an
example is shown by methods AddUpdateEventListener and AddConnectionListener.
The former links a handler for listening to a possible external event coming
from the DataProvider connection (typically connecting to MDSplus AST-CAN
requests), the latter is generally used to connect the gui progress bar
listening to the provider connection update events that inform the user of the
status of the signals download. 





The DataProvider implementations
--------------------------------

A diagram showing the basic inheritance tree of the DataProvider interface is
shown in Figure that follows.

\image html img/dataprovider_inheritance.jpg "DataProvider interface inheritance diagram "
\image latex img/dataprovider_inheritance.jpg "DataProvider interface inheritance diagram " height=3cm

The main implementation directly deriving from the interface is the /ref
jScope.MdsDataProvider, it represents the the standard object that provides
MDSplus data wave data to the scope. It relies to a pure java implementation of
the mdsip protocol (i.e. the MDSplus connection protocol) and talks to running
servers using TCP Thin clients MDSplus connections.


## LocalDataProvider

The \ref jScope.LocalDataProvider is implemented using JNI native functions
that call the low level functionalities of the MDSplus API in locally compiled
libraries. This makes possible to use a local defined data tree or an active
Distributed client set with the proper environment variable (see /ref
mdsip_manual).

## MdsDataProviderUDT

The new UDT connection was added in \ref jScope.MdsDataProviderUdt matching the
compiled MDSplus UDT implementation. The UDT protocol appears very effective in
the case of hi-latency long distant connections gaining speed rates several
times higher than TCP.



