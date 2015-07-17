
Data Type for Actions {#dt_action}
=====================

MDSplus provides an action data type used for describing actions to be
dispatched to action servers during a normal experiment cycle. An action
consists of 5 parts but the current implementations of action dispatching only
refernce the first three parts, a dispatch part, a task part and an errorlogs
part (OpenVMS only). The dispatch part (See: DTYPE\_DISPATCH) includes
information such as which server should perform the task, when during the
course of the shot cycle it should be executed, what MDSplus event should be
issued when the task completes. The task part describes what is to be done when
the action is executed. There are a couple different types of task types
currently support (See: DTYPE\_METHOD and DTYPE\_ROUTINE). The errorlogs part
is implemented only on OpenVMS. If this part contains a string consisting of a
comma delimited list of user accounts, each user on the list will receive a
OpenVMS cluster-wide broadcast message if this action fails during execution.
The remaining parts were originally defined to hold statistics from the
execution of the action but since this would require rewriting the action data
(which is usually considered setup information and not writable in the pulse
files) this feature was never used. This unused parts are called
completion\_message and performance.


The above example builds an action item consisting of a dispatch item and a
metho item. The dispatch item is specifying that this action should be run on
the action server called “CAMAC\_SERVER” during the “STORE” phase using
sequential scheduling, executed after all actions with sequence numbers between
1-49 have completed. When this action completes the dispatcher should issue the
MDSplus event called “A12\_42\_DONE”). The task portion of the action is a
method item specifying the “STORE” action is to be performed on the A14\_42
(tree node) device.

Normally actions are defined using tools such as the “traverser” and “actions”
applications and the contents of actions are referenced by action dispatchers
so you rarely need to build or access the parts of actions, dispatch and method
items by hand.

