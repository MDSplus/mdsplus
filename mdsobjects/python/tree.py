from data import Data,makeData
from threading import RLock,Thread
class Tree(object):
    """Open an MDSplus Data Storage Hierarchy"""

    _lock=RLock()

    def __init__(self,tree,shot):
        from _treeshr import TreeOpen
        try:
            Tree.lock()
            self.ctx=TreeOpen(tree,shot)
            Tree._activeTree=self
        finally:
            Tree.unlock()
        return

    def __getattr__(self,name):
        if name == 'shot':
            try:
                Tree.lock()
                self.restoreContext()
                ans = int(Data.execute('$shot'))
            finally:
                Tree.unlock()
            return ans
        if name == 'tree':
            try:
                Tree.lock()
                self.restoreContext()
                ans = str(Data.execute('$expt'))
            finally:
                Tree.unlock()
            return ans
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

    def doMethod(nid,method):
        """Support for PyDoMethod.fun used for python device support"""
        from treenode import TreeNode
        t=Tree(Data.execute('$EXPT').value,Data.execute('$shot').value)
        n=TreeNode(nid,t)
        top=n.conglomerate_nids[0]
        c=top.record
        q=c.qualifiers
        model=c.model
        for i in range(len(q)):
            exec str(q[0])
        try:
            exec str('makeData('+model+'(top).'+method+'(Data.getTdiVar("__do_method_arg__"))).setTdiVar("_result")')
        except AttributeError,e:
            makeData(0xfd180b0).setTdiVar("_result")
        except Exception,e:
            print "Error doing %s on node %s" % (str(method),str(n))
            makeData(0).setTdiVar("_result")
            raise
        return Data.getTdiVar("_result")
    doMethod=staticmethod(doMethod)

    def restoreContext(self):
        """Use internal context associated with this tree. Internal use only."""
        from _treeshr import TreeRestoreContext
        Tree._activeTree=self
        return TreeRestoreContext(self.ctx)

    def __del__(self):
        try:
            tc=TreeCtxCleanup(self.ctx)
            tc.start()
        except:
            pass
        return

    def getDefault(self):
        """Return current default TreeNode"""
        from _treeshr import TreeGetDefault
        from treenode import TreeNode
        try:
            Tree.lock()
            ans = TreeNode(TreeGetDefault(self.ctx))
        finally:
            Tree.unlock()
        return ans
            

    def setDefault(self,node):
        """Set current default TreeNode."""
        from _treeshr import TreeSetDefault
        from treenode import TreeNode
        if isinstance(node,TreeNode):
            if node.tree is self:
                TreeSetDefault(self.ctx,node.nid)
            else:
                raise TypeError,'TreeNode must be in same tree'
        else:
            raise TypeError,'default node must be a TreeNode'

    def getNode(self,name):
        """Locate node in tree. Returns TreeNode if found. Use double backslashes in node names."""
        from treenode import TreeNode
        from _treeshr import TreeFindNode
        if isinstance(name,int):
            return TreeNode(name,self)
        else:
            try:
                Tree.lock()
                ans=TreeNode(TreeFindNode(self.ctx,str(name)),self)
            finally:
                Tree.unlock()
            return ans
        
    def lock(cls):
        cls._lock.acquire()
    lock=classmethod(lock)

    def unlock(cls):
        cls._lock.release()
    unlock=classmethod(unlock)

    def getNodeWild(self,name,*usage):
        """Find nodes in tree using a wildcard specification. Returns TreeNodeArray if nodes found."""
        from treenode import TreeNodeArray
        try:
            Tree.lock()
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
        finally:
            Tree.unlock()
        return TreeNodeArray(nids)

class TreeCtxCleanup(Thread):
    """Close trees and free tree ctx in thread when tree instance is deleted"""
    def __init__(self,ctx):
        Thread.__init__(self)
        self.ctx=ctx

    def run(self):
        from _treeshr import TreeCloseAll,TreeFree,TreeException
        if hasattr(self,'ctx'):
            try:
                Tree.lock()
                TreeCloseAll(self.ctx)
            finally:
                try:
                    Tree.unlock()
                    TreeFree(self.ctx)
                except:
                    pass
        
