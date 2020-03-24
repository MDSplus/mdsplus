from MDSplus import *
def checkMarte(treeName, nid):
  print('Ciao sono checkMarte')
  print(nid)
  t = Tree(treeName, -1)
  node = TreeNode(nid, t)
  cong = node.getData()
  deviceClass = cong.getDevice()
  device = deviceClass(node)
  return device.check()
 
