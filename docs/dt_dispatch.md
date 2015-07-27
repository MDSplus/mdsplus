
Dispatch {#dt_dispatch}
========

MDSplus provides an dispatch data type used for specifying the dispatching
information of an action. This determines where and when an automatic action
should be executed during the course of a experiment cycle.

A dispatch item is a structure consisting of 5 parts; dispatch type, server,
phase, when, and completion. The dispatch type part specifies the type of
scheduling to be done. Currently only one type of dispatching is supported in
MDSplus and that is sequential scheduling (value=2). This field should contain
an integer value of 2.

The server part specifies the server that should execute the action. For tcpip
based action servers this field should contain a string such as host:port where
host is the tcpip hostname of the computer where the action server is running
and the port is the port number on which the action server is listening for
actions to perform. For DECNET based action servers (OpenVMS only), this should
be a string such as host::object where the host is the DECNET node name and the
object is the DECNET object name of the action server.

The phase part of a dispatch item is either the name or number corresponding to
the phase of the experiment cycle. These would normally be phases such as
"store", "init","analysis" etc. but the names and numbers of the phases can be
customized by the MDSplus system administrator by modifying the TDI function
phase_table().

The when part normally contains either an integer value or an expression which
evaluates to an integer value representing a sequence number. When the
dispatcher (implemented by a set of mdstcl dispatch commands) builds a
dispatching table, it finds all the actions defined in a tree and then sorts
these actions first by phase and then by sequence number. Actions are then
dispatched to servers during a phase in order of their sequence numbers (except
for actions with sequence numbers less than or equal to zero which are not
dispatched). There is a special case for the when part which enables you to set
up dependencies on other actions. If instead of specifying a sequence number
for the when part, you specify an expression which references other action
nodes in the tree, this action will not be dispatched until all action nodes
referenced in the expression have completed. When all the actions referenced in
the expression have completed, the expression is then evaluated substituting
the completion status of the referenced actions instead of the action node
itself. If the result of the evaluation yields an odd number (low bit set) then
this action will be dispatched. If the result is an even value then this action
is not dispatched but instead assigned a failure completion status in case
other actions have when expressions refering to it. Using this mechanism you
can configure fairly complex conditional dispatching.

The completion part can hold a string defining the name of an MDSplus event to
be declared upon completion of this action. These events are often used to
trigger updates on visualization tools such as dwscope when this action
completes indicating availability of the data.

The following shows some examples of creating and accessing an MDSplus action.
These examples are written in TDI (the MDSplus expression evaluator language).

    _MYACTION = BUILD_ACTION(
             BUILD_DISPATCH(2,"CAMAC_SERVER","STORE",50,"A12_42_DONE"),
             BUILD_METHOD(*,"STORE",A12_42)
            )
    _DISPATCH = DISPATCH_OF(_MYACTION)
    _TASK = TASK_OF(_MYACTION)

The above example builds an action item consisting of a dispatch item and a
metho item. The dispatch item is specifying that this action should be run on
the action server called "CAMAC_SERVER" during the "STORE" phase using
sequential scheduling, executed after all actions with sequence numbers between
1-49 have completed. When this action completes the dispatcher should issue the
MDSplus event called "A12_42_DONE"). The task portion of the action is a method
item specifying the "STORE" action is to be performed on the A14_42 (tree node)
device.

Normally actions are defined using tools such as the "traverser" and "actions"
applications and the contents of actions are referenced by action dispatchers
so you rarely need to build or access the parts of actions, dispatch and method
items by hand.


The following table lists some of the TDI functions available for creating and
accessing actions:

\latexonly { \tiny \endlatexonly


| **Function**    | **Description**                                       |
|-----------------|-------------------------------------------------------|
| BUILD\_DISPATCH | Build a dispatch structure                            |
| COMPLETION\_OF  | Return the completion part (event) of a dispatch item |
| IDENT\_OF       | Return the server part of a dispatch item             |
| MAKE\_DISPATCH  | Make an dispatch structure                            |
| PHASE\_OF       | Return the phase part of a dispatch item              |
| WHEN\_OF        | Return the when part of a dispatch item               |


\latexonly } \endlatexonly

