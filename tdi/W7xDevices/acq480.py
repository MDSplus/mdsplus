import MDSplus
import acqfmc

class ACQ480 (acqfmc.ACQFMC) :
    """
    D-tAcq ACQ480FMC 8 channel transient recorder
    http://www.d-tacq.com/modproducts.shtml
    """

    parts = []
    parts.extend(acqfmc.ACQFMC.parts_head)
    parts.extend(acqfmc.ACQFMC.parts_tail)
    parts.append({'path': ':CLOCK_FREQ',  'type': 'numeric', 'value': 10000000, 'options': ('no_write_shot',)})
    parts.append({'path': ':ACTIVE_CHAN', 'type': 'numeric', 'value': 8,        'options': ('no_write_shot',)})
    for i in range(8) :
        parts.append({'path': ':INPUT_%02d'          % (i + 1,), 'type': 'signal',  'options': ('no_write_model', 'write_once',)})
        parts.append({'path': ':INPUT_%02d:STARTIDX' % (i + 1,), 'type': 'numeric', 'options': ('no_write_shot')})
        parts.append({'path': ':INPUT_%02d:ENDIDX'   % (i + 1,), 'type': 'numeric', 'options': ('no_write_shot')})
        parts.append({'path': ':INPUT_%02d:INC'      % (i + 1,), 'type': 'numeric', 'options': ('no_write_shot')})
