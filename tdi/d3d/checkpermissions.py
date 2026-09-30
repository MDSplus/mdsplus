from os import getgroups
from grp import getgrgid


import MDSplus
EFITS = ["efit%02d" % (i) for i in range(4,21) ]
NON_WRITEABLE_TREES = set(["efit"]+EFITS)

def checkpermissions(treename):
    try:
        USERGROUPS = set(getgrgid(group).gr_name.lower() for group in getgroups()) #a set of all the group names this user belongs to.
        treename = str(treename).lower()
        if (not treename in NON_WRITEABLE_TREES) and treename in USERGROUPS:
            return MDSplus.Int32(1)
        else:
            return MDSplus.Int32(0)
    except Exception as exc:
        import traceback
        traceback.print_exc()
        return MDSplus.Int32(0)

