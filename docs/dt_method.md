Method {#dt_method}
======

MDSplus provides a method data type for describing an operation to be performed
on an MDSplus device to be used as the task portion of an action data item.
MDSplus devices support various "methods" which can be performed on the device.
For example, transient digitizer device may support an "INIT" method to prepare
the device to acquire data and a "STORE" method which would retrieve the data
from the physical device and store it into the device nodes in the MDSplus
tree.

The method data type is a structure consisting of three fields followed by
optional arguments to the method being performed.

The first field is a timeout value specified in seconds. When the method is
being performed on the device and a timeout is specified, a timer is started
when the operation is started and if the operation is not completed before the
specified timeout, the operation will be aborted abruptly. This field can
either be omitted or should contain a value or expression which when evaluated
will result in a scalar numeric value.

The next field in the method data type is the name of the method to be
performed. This should contain a value or expression which when evaluated will
result in a scalar text value representing a supported method for the device
being operated on (i.e. "INIT", "STORE","EXECUTE"). The set of available
methods is device specific.

The next field is the object on which this operation is to be performed. This
should be a node reference to one of the nodes which make up the device,
usually the top node in the device node structure.

These first three fields can be followed by optional parameters which will be
interpretted by the implementation of the method. These fields are specific to
the implementation of the various methods.

The following shows an example of creating an MDSplus method. This example is
written in TDI (the MDSplus expression evaluator language).

    _MYACTION = BUILD_ACTION(
             BUILD_DISPATCH(2,"CAMAC_SERVER","STORE",50,"A12_42_DONE"),
             BUILD_METHOD(*,"STORE",A12_42)
            )

The above example builds an action item consisting of a dispatch item and a
method item. The dispatch item is specifying that this action should be run on
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
accessing method:


\latexonly { \tiny \endlatexonly

| **Function**  | **Description**                         |
|---------------|-----------------------------------------|
| BUILD\_METHOD | Build a method structure                |
| MAKE\_METHOD  | Make a method structure                 |
| METHOD\_OF    | Return the method name part of a method |
| OBJECT\_OF    | Return the object part of a method      |
| TIME\_OUT\_OF | Return the timeout part of a method     |

\latexonly } \endlatexonly
