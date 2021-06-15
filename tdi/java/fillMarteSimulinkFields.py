from MDSplus import *


def fillMarteSimulinkFields(treeName, nid):
    print('Filling fields...')
    t = Tree(treeName, -1)
    node = TreeNode(nid, t)
    cong = node.getData()
    deviceClass = cong.getDevice()
    device = deviceClass(node)
    try:
        device.getConfigurationFromSimulink()
    except Exception as exc:
        return str(exc)
    return 'Fields filled'
