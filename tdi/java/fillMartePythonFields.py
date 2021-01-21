from MDSplus import *


def fillMartePythonFields(treeName, nid):
    print('Filling fields...')
    t = Tree(treeName, -1)
    node = TreeNode(nid, t)
    cong = node.getData()
    deviceClass = cong.getDevice()
    device = deviceClass(node)
    try:
        device.getConfigurationFromPython()
    except Exception as exc:
        return str(exc)
    return 'Fields filled'
