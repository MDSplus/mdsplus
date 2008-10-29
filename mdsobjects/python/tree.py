from MDSobjects.data import Data
class Tree(object):
    """Open an MDSplus Data Storage Hierarchy"""

    def __init__(self,tree,shot):
        from MDSobjects._treeshr import TreeOpen
        self.ctx=TreeOpen(tree,shot)
        Tree._activeTree=self
        return

    def __getattr__(self,name):
        if name == 'shot':
            self.restoreContext()
            return int(Data.execute('$shot'))
        if name == 'tree':
            self.restoreContext()
            return str(Data.execute('$expt'))
        if name == 'default':
            return self.getDefault()
        else:
            raise AttributeError,'No such attribute: '+name

    def __setattr__(self,name,value):
        if name in ('shot','tree'):
            raise AttributeError,'Read only attribute: '+name
        if name == 'default':
            self.setDefault(value)
        self.__dict__[name]=value

    def __str__(self):
        return 'Tree("%s",%d)' % (self.tree,self.shot)

    def restoreContext(self):
        """Use internal context associated with this tree. Internal use only."""
        from MDSobjects._treeshr import TreeRestoreContext
        Tree._activeTree=self
        return TreeRestoreContext(self.ctx)

    def __del__(self):
        from MDSobjects._treeshr import TreeRestoreContext,TreeClose,TreeFree
        oldctx=TreeRestoreContext(self.ctx)
        done=False
        while not done:
            try:
                TreeClose(None,0)
            except Exception,e:
                done=True
        TreeFree(self.ctx)
        TreeRestoreContext(oldctx)

    def getDefault(self):
        """Return current default TreeNode"""
        from MDSobjects._treeshr import TreeGetDefault
        from MDSobjects.treenode import TreeNode
        self.restoreContext()
        return TreeNode(TreeGetDefault())

    def setDefault(self,node):
        """Set current default TreeNode."""
        from MDSobjects._treeshr import TreeSetDefault
        from MDSobjects.treenode import TreeNode
        if isinstance(node,TreeNode):
            if node.tree is self:
                self.restoreContext()
                TreeSetDefault(node.nid)
            else:
                raise TypeError,'TreeNode must be in same tree'
        else:
            raise TypeError,'default node must be a TreeNode'

    def getNode(self,name):
        """Locate node in tree. Returns TreeNode if found. Use double backslashes in node names."""
        from MDSobjects.treenode import TreeNode
        from MDSobjects._treeshr import TreeFindNode
        self.restoreContext()
        if isinstance(name,int):
            return TreeNode(name,self)
        else:
            return TreeNode(TreeFindNode(str(name)),self)

    def getNodeWild(self,name,*usage):
        """Find nodes in tree using a wildcard specification. Returns TreeNodeArray if nodes found."""
        from MDSobjects.treenode import TreeNodeArray
        self.restoreContext()
        if len(usage) > 0:
            from numpy import array
            for i in range(len(usage)):
                if not isinstance(usage[i],str):
                    raise TypeError,'Usage arguments must be strings'
            usage=array(usage)
            nids=Data.compile('getnci($,"NID_NUMBER",$)',name,usage).evaluate()
        else:
            nids=Data.compile('getnci($,"NID_NUMBER")',(name,)).evaluate()
        return TreeNodeArray(nids)
            
            
        
        
