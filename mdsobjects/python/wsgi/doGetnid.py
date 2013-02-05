def doGetnid(self):
    self.openTree(self.path_parts[1],self.path_parts[2])
    output=str(self.treeObj.getNode(self.args['node'][-1]).nid)
    return ('200 OK',[('Content-type','text/text')],output)

