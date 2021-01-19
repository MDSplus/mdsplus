
from MDSplus import *


class STREAM_SETUP(Device):
    parts = [{'path': ':COMMENT', 'type': 'text'}]
    for chanId in range(1, 65):
        chanName = 'CHAN_'+format(chanId, '02d')
        parts.append({'path': '.'+chanName, 'type': 'structure'})
        parts.append({'path': '.'+chanName+':NAME', 'type': 'text'})
        parts.append({'path': '.'+chanName+':NID', 'type': 'numeric'})
        parts.append({'path': '.'+chanName+':GAIN',
                      'type': 'numeric', 'value': 1})
        parts.append({'path': '.'+chanName+':OFFSET',
                      'type': 'numeric', 'value': 0})
    parts.append({'path': ':INIT_ACTION', 'type': 'action',
                  'valueExpr': "Action(Dispatch('CPCI_SERVER','PULSE_PREPARATION',50,None),Method(None,'init',head))",
                  'options': ('no_write_shot',)})

    def init(self):
        print('================= STREAM_SETUP Init ===============')
        for chan in range(0, 64):
            try:
                name = getattr(self, 'chan_%02d_name' % (chan+1)).getString()
                nid = getattr(self, 'chan_%02d_nid' % (chan+1)).getData()
                try:
                    gain = getattr(self, 'gain%d' % (chan+1)).getData()
                except:
                    gain = Float32(1.)
                try:
                    offset = getattr(self, 'chan_%02d_offset' %
                                     (chan+1)).getData()
                except:
                    offset = Float32(0.)
                if isinstance(nid, TreeNode):
                    nid.setExtendedAttribute('STREAM_NAME', name)
                    nid.setExtendedAttribute('STREAM_GAIN', gain)
                    nid.setExtendedAttribute('STREAM_OFFSET', offset)
                    print(nid, name, gain, offset)
            except:
                pass
        return 1
