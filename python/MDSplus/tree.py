#
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#


def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())


import ctypes as _C
import numpy as _N
import threading as _threading
import gc as _gc
import os as _os
import sys as _sys
import time as _time
#### Load base python modules referenced ###
#
_ver = _mimport('version')
_dat = _mimport('mdsdata')
_exc = _mimport('mdsExceptions')
#### Load Shared Libraries Referenced #######
#
_TreeShr = _ver.load_library('TreeShr')
_XTreeShr = _ver.load_library('XTreeShr')
_TreeShr.TreeCtx.restype = _C.c_void_p
_TreeShr.TreeDbid.restype = _C.c_void_p

#############################################
#### Load other python modules referenced ###
#
_arr = _mimport('mdsarray')
_scr = _mimport('mdsscalar')
_apd = _mimport('apd')
_dcl = _mimport('mdsdcl')
_cmp = _mimport('compound')
_dsc = _mimport('descriptor')
_mds = _mimport('_mdsshr')
_con = _mimport('connection')
#
#############################################


class classmethodX(object):
    def __get__(self, inst, cls):
        if inst is None:
            return self.method.__get__(cls)
        else:
            return self.method.__get__(inst)

    def __init__(self, method):
        self.method = method

#### hidden module variables ################
#


_usage_table = {'ANY': 0, 'NONE': 1, 'STRUCTURE': 1, 'ACTION': 2,      # Usage name to codenum table
                'DEVICE': 3, 'DISPATCH': 4, 'NUMERIC': 5, 'SIGNAL': 6,
                'TASK': 7, 'TEXT': 8, 'WINDOW': 9, 'AXIS': 10,
                'SUBTREE': 11, 'COMPOUND_DATA': 12}


class UsageError(KeyError):
    def __init__(self, usage):
        super(UsageError, self).__init__('Invalid usage "%s". Must be one of: %s' % (
            str(usage), ', '.join(_usage_table.keys())))

_compression_methods_table = {'standard': 0, 'gzip': 1}

class CompressionMethodError(KeyError):
    def __init__(self, compression_method):
        super(CompressionMethodError, self).__init__('Invalid compression_method "%s". Must be one of: %s' % (
            str(compression_method), ', '.join(_compression_methods_table.keys())))


#
###################################################


class TreeNodeException(_exc.MDSplusException):
    pass


def trace():
    import inspect
    frame = inspect.currentframe().f_back
    s = ""
    while frame:
        s = "%-20s %s,%d\n" % (frame.f_code.co_name,
                               frame.f_code.co_filename, frame.f_lineno)+s
        frame = frame.f_back
    return s


def _getNodeByAttr(self, name):
    if name.startswith('_'):
        namesplit = name.split('__', 1)
        if len(namesplit) == 2 and namesplit[1] == namesplit[1].upper():
            return self.getNode('\\%s::%s' % tuple(namesplit[1].split('__', 1)+['TOP'])[:2])
        if name.upper() == name:
            return self.getNode('\\%s' % name[1:])
    elif name.upper() == name:
        return self.getNode(name)
    raise _exc.TreeNNF

############### DBI Options ######


class Dbi(object):
    NAME = (1, str, 12)
    SHOTID = (2, int,  4)
    MODIFIED = (3, bool, 4)
    OPEN_FOR_EDIT = (4, bool, 4)
    INDEX = (5, int,  4)
    NUMBER_OPENED = (6, int,  4)
    MAX_OPEN = (7, int,  4)  # settable
    DEFAULT = (8, str, 256)
    OPEN_READONLY = (9, bool, 4)
    VERSIONS_IN_MODEL = (10, bool, 4)  # settable
    VERSIONS_IN_PULSE = (11, bool, 4)  # settable
    DISPATCH_TABLE = (13, bool, 4)

    class _dbi_item(_C.Structure):
        """ Ctype structure class for making calls into _TreeGetDbi() """
        _fields_ = [("buflen", _C.c_ushort),
                    ("code", _C.c_ushort),
                    ("pointer", _C.c_char_p),
                    ("retlen", _C.c_void_p),
                    ("end0", _C.c_ushort),
                    ("end1", _C.c_ushort),
                    ("ent2", _C.c_void_p),
                    ("end3", _C.c_void_p), ]

    def __init__(self, code, pointer):
        super(Dbi._DBI_ITEM, self).__init__(
            4, code, pointer, _C.c_void_p(_C.c_int32(0)), 0, 0, 0, 0)

    @staticmethod
    def _dbiProp(info, doc=None, settable=False):
        def getter(self):
            return self._getDbi(info)
        if settable:
            def setter(self, value):
                return self._setDbi(info, value)
            if isinstance(doc, str):
                doc = "%s (settable)" % doc
        else:
            setter = None
        return property(getter, setter, doc=doc)

############### Node Characteristic Options ######
#


class Flags(object):
    STATE = 0x00000001
    PARENT_STATE = 0x00000002
    ESSENTIAL = 0x00000004
    CACHED = 0x00000008
    VERSIONS = 0x00000010
    SEGMENTED = 0x00000020
    SETUP_INFORMATION = 0x00000040
    WRITE_ONCE = 0x00000080
    COMPRESSIBLE = 0x00000100
    DO_NOT_COMPRESS = 0x00000200
    COMPRESS_ON_PUT = 0x00000400
    NO_WRITE_MODEL = 0x00000800
    NO_WRITE_SHOT = 0x00001000
    PATH_REFERENCE = 0x00002000
    NID_REFERENCE = 0x00004000
    INCLUDE_IN_PULSE = 0x00008000
    COMPRESS_SEGMENTS = 0x00010000

    def __init__(self, flags):
        self.flags = flags

    @property
    def state(self): return self.flags & Flags.STATE != 0

    @property
    def parent_state(self): return self.flags & Flags.PARENT_STATE != 0

    @property
    def essential(self): return self.flags & Flags.ESSENTIAL != 0

    @property
    def chached(self): return self.flags & Flags.CACHED != 0

    @property
    def versions(self): return self.flags & Flags.VERSIONS != 0

    @property
    def segmented(self): return self.flags & Flags.SEGMENTED != 0

    @property
    def setup_information(
        self): return self.flags & Flags.SETUP_INFORMATION != 0

    @property
    def write_once(self): return self.flags & Flags.WRITE_ONCE != 0

    @property
    def comressible(self): return self.flags & Flags.COMPRESSIBLE != 0

    @property
    def do_not_compress(self): return self.flags & Flags.DO_NOT_COMPRESS != 0

    @property
    def compress_on_put(self): return self.flags & Flags.COMPRESS_ON_PUT != 0

    @property
    def no_write_model(self): return self.flags & Flags.NO_WRITE_MODEL != 0

    @property
    def no_write_shot(self): return self.flags & Flags.NO_WRITE_SHOT != 0

    @property
    def path_reference(self): return self.flags & Flags.PATH_REFERENCE != 0

    @property
    def nid_reference(self): return self.flags & Flags.NID_REFERENCE != 0

    @property
    def include_in_pulse(self): return self.flags & Flags.INCLUDE_IN_PULSE != 0

    @property
    def compress_segments(
        self): return self.flags & Flags.COMPRESS_SEGMENTS != 0

    @classmethod
    def _nciFlag(cls, mask, doc=None):
        def getter(self):
            return (int(self.get_flags) & mask) != 0
        if mask == cls.STATE:
            def setter(self, value): self.on = not value
        else:
            def setter(self, value):
                return self._setNciFlag(mask, value)
        if isinstance(doc, str):
            doc = "%s (settable)" % doc
        return property(getter, setter, doc=doc)


class Nci(object):
    _IS_CHILD = 1
    _IS_MEMBER = 2
    SET_FLAGS = (1, _C.c_uint32, 4, int)
    CLEAR_FLAGS = (2, _C.c_uint32, 4, int)
    TIME_INSERTED = (4, _C.c_uint64, 8, _scr.Uint64)
    OWNER_ID = (5, _C.c_uint32, 4, int)
    CLASS = (6, _C.c_uint8, 1, int)
    DTYPE = (7, _C.c_uint8, 1, int)
    LENGTH = (8, _C.c_uint32, 4, int)
    STATUS = (9, _C.c_uint32, 4, int)
    CONGLOMERATE_ELT = (10, _C.c_uint16, 2, int)
    GET_FLAGS = (12, _C.c_uint32, 4, int)
    NODE_NAME = (13, _C.c_char_p, 13, str)
    PATH = (14, _C.c_char_p, 1024, str)
    DEPTH = (15, _C.c_uint32, 4, int)
    PARENT = (16, _C.c_int32, 4, None)
    BROTHER = (17, _C.c_int32, 4, None)
    MEMBER = (18, _C.c_int32, 4, None)
    CHILD = (19, _C.c_int32, 4, None)
    PARENT_RELATIONSHIP = (20, _C.c_uint32, 4, int)
    CONGLOMERATE_NIDS = (21, _C.c_int32*1024, 1024*4, None)
    ORIGINAL_PART_NAME = (22, _C.c_char_p, 1024, str)
    NUMBER_OF_MEMBERS = (23, _C.c_uint32, 4, int)
    NUMBER_OF_CHILDREN = (24, _C.c_uint32, 4, int)
    MEMBER_NIDS = (25, _C.c_int32*4096, 4096*4, None)
    CHILDREN_NIDS = (26, _C.c_int32*4096, 4096*4, None)
    FULLPATH = (27, _C.c_char_p, 1024, str)
    MINPATH = (28, _C.c_char_p, 1024, str)
    USAGE = (29, _C.c_uint8, 1, int)
    PARENT_TREE = (30, _C.c_char_p, 13, str)
    RLENGTH = (31, _C.c_uint32, 4, int)
    NUMBER_OF_ELTS = (32, _C.c_uint32, 4, int)
    DATA_IN_NCI = (33, _C.c_uint32, 4, bool)
    ERROR_ON_PUT = (34, _C.c_uint32, 4, bool)
    RFA = (35, _C.c_uint64, 8, _scr.Uint64)
    IO_STATUS = (36, _C.c_uint32, 4, int)
    IO_STV = (37, _C.c_uint32, 4, int)
    DTYPE_STR = (38, _C.c_char_p, 64, str)
    USAGE_STR = (39, _C.c_char_p, 64, str)
    CLASS_STR = (40, _C.c_char_p, 64, str)
    VERSION = (41, _C.c_uint32, 4, int)
    COMPRESSION_METHOD = (42, _C.c_uint8, 1, int)
    COMPRESSION_METHOD_STR = (43, _C.c_char_p, 64, str)

    class _nci_item(_C.Structure):
        _fields_ = [("buflen", _C.c_ushort),
                    ("code",   _C.c_ushort),
                    ("pointer", _C.c_char_p),
                    ("retlen", _C.POINTER(_C.c_int32)),
                    ("end0", _C.c_ushort),
                    ("end1", _C.c_ushort),
                    ("end2", _C.c_void_p),
                    ("end3", _C.c_void_p)]

        def __init__(self, buflen, code, pointer):
            super(Nci._nci_item, self).__init__(buflen, code,
                                                pointer, _C.pointer(_C.c_int32(0)), 0, 0, 0, 0)

    @staticmethod
    def _nciProp(info, doc=None):
        if isinstance(info, tuple):
            def getter(self):
                return self._getNci(info)
        else:
            def getter(self):
                return (int(self.get_flags) & info) != 0
        return property(getter, doc=doc)
#
#################################################################


class Tree(object):
    """Open an MDSplus Data Storage Hierarchy"""

    _lock = _threading.RLock()
    public = False
    _id = 0
    path = None
    _ctx = None

    @property
    def pctx(self):
        return _C.c_void_p(_TreeShr.TreeCtx()) if self.public else _C.byref(self._ctx)

    @property
    def ctx(self):
        return _C.c_void_p(_TreeShr.TreeDbid()) if self.public else self._ctx

    @staticmethod
    def getShotDB(expt, path=None, lower=None, upper=None):
        """
        getShotDB("mytree")
        getShotDB("mytree",n)                                # only n-th entry of mytree_path
        getShotDB("mytree","/my/local/tree/path")            # only look in local path
        getShotDB("mytree","myserver::")                     # only look on server
        getShotDB("mytree","myserver::/my/remote/tree/path") # only look in path on server
        getShotDB("mytree",lower=_from)                      # only shots >= _from
        getShotDB("mytree",upper=_upto)                      # only shots <= _upto
        getShotDB("mytree",lower=_from,upper=_upto)          # only shots from _from upto _upto
        """
        if isinstance(expt, _dat.Data):
            expt = expt.data()
        expt = str(expt).lower()

        def getTreePath():
            # split path variable into single paths and replaces the ~t treename
            expt_paths = _mds.getenv('%s_path' % expt)
            if expt_paths is None:
                expt_paths = _mds.getenv('default_tree_path')
            if expt_paths is None:
                raise _exc.TreeNOPATH
            return expt_paths.replace('~t', expt).split(';')

        def getshots(expt_path, lower, upper):
            if expt_path.find('::') >= 0:
                # path is referring to remote tree
                server, path = expt_path.split('::', 2)
                # check if thick or distributed
                path = "*" if len(path) == 0 else '"'+path+'"'
                # handle None for upper and lower
                if lower is None:
                    lower = "*"
                if upper is None:
                    upper = "*"
                # fetch data from server
                return _con.Connection(server).get('getShotDb("%s",%s,%s,%s)' % (expt, path, str(lower), str(upper)),).data().tolist()
            start = expt+'_'
            files = [f[len(expt):-5].split('_') for f in _os.listdir(expt_path)
                     if f.endswith('.tree') and f.startswith(start)]
            return [int(f[1]) for f in files if len(f) == 2 and f[1] != 'model']
        """The path argument is interpreted"""
        # try to convert to native datatype
        if isinstance(path, _dat.Data):
            path = path.data().tolist()
        if isinstance(path, (int,)):
            # path is int and refering to the index of the path list
            path = getTreePath()[path]
        if isinstance(path, _ver.basestring):
            # path is str and used as path
            shots = getshots(path, lower, upper)
        else:
            # path is undefined and the total list will be collected
            shots = []
            for expt_path in getTreePath():
                try:
                    shots += getshots(expt_path, lower, upper)
                except:
                    pass  # may happen if path not reachable
        """filter result by upper and lower limits"""
        if lower is not None or upper is not None:
            if lower is None:
                shots = _N.array(filter(lambda x: x <= int(upper), shots))
            elif upper is None:
                shots = _N.array(filter(lambda x: x >= int(lower), shots))
            else:
                shots = _N.array(filter(lambda x: x >= int(
                    lower) and x <= int(upper), shots))
        shots.sort()
        return shots

    def getDatafileSize(self):
        """return data file size.
        @rtype: long
        """
        _TreeShr._TreeGetDatafileSize.restype = _C.c_int64
        return _TreeShr._TreeGetDatafileSize(self.ctx)

    @classmethodX
    def cleanDatafile(self, tree=None, shot=None):
        """Clean up data file.
        @rtype: None
        """
        if isinstance(self, (Tree,)):
            tree, shot = self.tree, self.shot
            pctx = self.pctx
        else:
            pctx = _C.c_void_p(_TreeShr.TreeCtx())
        _exc.checkStatus(
            _TreeShr._TreeCleanDatafile(pctx,
                                        _C.c_char_p(_ver.tobytes(tree)),
                                        _C.c_int32(int(shot))))

    @classmethodX
    def compressDatafile(self, tree=None, shot=None):
        """Compress data file.
        @rtype: None
        """
        if isinstance(self, (Tree,)):
            tree, shot = self.tree, self.shot
            pctx = self.pctx
        else:
            pctx = _C.c_void_p(_TreeShr.TreeCtx())
        _exc.checkStatus(
            _TreeShr._TreeCompressDatafile(pctx,
                                           _C.c_char_p(_ver.tobytes(tree)),
                                           _C.c_int32(int(shot))))

    @classmethodX
    def getFileName(self, tree=None, shot=None):
        """Return file path.
        @rtype: str
        """
        xd = _dsc.DescriptorXD()
        if tree is None:
            treeref = None
        else:
            # even with ctx, you may address subtrees
            treeref = _C.c_char_p(_ver.tobytes(tree))
        if isinstance(self, (Tree,)):
            _exc.checkStatus(
                _TreeShr._TreeFileName(self.ctx,
                                       treeref, _C.c_int32(0), xd.ref))
        else:
            _exc.checkStatus(
                _TreeShr.TreeFileName(
                    treeref, _C.c_int32(int(shot)), xd.ref))
        return _ver.tostr(xd.value)

    def copy(self, mode='NORMAL'):
        """returns a local private instance of the tree opend in specified mode
        @param mode: Optional mode, one of 'Normal','Edit','New','Readonly'
        @type mode: str
        @rtype: Tree
        """
        return Tree(self.tree, self.shot, mode)

    def readonly(self, shot=None):
        self.open('READONLY', shot)

    def edit(self, shot=None):
        self.open('EDIT', shot)

    def normal(self, shot=None):
        self.open('NORMAL', shot)

    def open(self, mode='NORMAL', shot=None):
        if shot is not None:
            self.shot = shot
        try:
            env_name = '%s_path' % self.tree.lower()

            if not self.path is None:
                old_path = _mds.getenv(env_name)
                _mds.setenv(env_name, self.path)
            mode = mode.upper()
            if mode == 'NORMAL':
                status = _TreeShr._TreeOpen(self.pctx,
                                            _C.c_char_p(
                                                _ver.tobytes(self.tree)),
                                            _C.c_int32(self.shot),
                                            _C.c_int32(0))
            elif mode == 'EDIT':
                status = _TreeShr._TreeOpenEdit(self.pctx,
                                                _C.c_char_p(
                                                    _ver.tobytes(self.tree)),
                                                _C.c_int32(self.shot),
                                                _C.c_int32(0))
            elif mode == 'READONLY':
                status = _TreeShr._TreeOpen(self.pctx,
                                            _C.c_char_p(
                                                _ver.tobytes(self.tree)),
                                            _C.c_int32(self.shot),
                                            _C.c_int32(1))
            elif mode == 'NEW':
                status = _TreeShr._TreeOpenNew(self.pctx,
                                               _C.c_char_p(
                                                   _ver.tobytes(self.tree)),
                                               _C.c_int32(self.shot))
            else:
                raise TypeError(
                    'Invalid mode specificed, use "normal","edit","new" or "readonly".')
            _exc.checkStatus(status)
            if not self.public:
                self.tree = self.name
                self.shot = self.shotid
        finally:
            if not self.path is None:
                _mds.setenv(env_name, old_path)

    def __init__(self, tree=None, shot=-1, mode='NORMAL', path=None):
        """Create a Tree instance. Specify a tree and shot and optionally a mode.
        If providing the mode argument it should be one of the following strings:
        'Normal','Edit','New','ReadOnly'.
        If no arguments provided, create instance of the active tree. (i.e. Tree())
        @param tree: Name of tree to open
        @type tree: str
        @param shot: Shot number
        @type shot: int
        @param mode: Optional mode, one of 'Normal','Edit','New','Readonly'
        @type mode: str
        """
        self.public = tree is None
        if not self.public:
            if path is not None:
                self.path = path
            self._ctx = _C.c_void_p(0)
            self.tree = tree
            self.shot = shot
            self.public = False
            self.open(mode)

    # support for the with-structure
    def __enter__(self):
        """ referenced if using "with Tree() ... " block"""
        return self

    def __del__(self):
        if not self.public and _TreeShr is not None:
            self.__exit__()
            _TreeShr.TreeFreeDbid(self._ctx)

    def __exit__(self, *args):
        """ Cleanup for with statement. If tree is open for edit close it. """
        try:
            if self.open_for_edit:
                self.quit()
            else:
                self.close()
        except:
            pass

    def quit(self):
        """Close edit session discarding node structure and tag changes.
        @rtype: None
        """
        with self._lock:
            _exc.checkStatus(_TreeShr._TreeQuitTree(self.pctx, 0, 0))

    def close(self):
        """Close tree.
        @rtype: None
        """
        _exc.checkStatus(_TreeShr._TreeClose(self.pctx, 0, 0))

    def _getDbi(self, info):
        """Return dbi data"""
        code, rtype, buflen = info
        if rtype is str:
            ans = _C.c_char_p(b' '*buflen)
            pointer = ans
        else:
            ans = _C.c_int32()
            pointer = _C.cast(_C.pointer(ans), _C.c_char_p)
        item = Dbi._dbi_item(buflen, code, pointer)
        _exc.checkStatus(_TreeShr._TreeGetDbi(self.ctx, _C.byref(item)))

        if rtype is str:
            return _ver.tostr(ans.value)
        return rtype(ans.value)

    def _setDbi(self, info, value):
        code, rtype, buflen = info
        if rtype is str:
            value = _ver.tobytes(value)
            ans = _C.c_char_p(value)
            pointer = ans
            buflen = len(value)+1
        else:
            if rtype == bool:
                value = 1 if value else 0
            ans = _C.c_int32(int(value))
            pointer = _C.cast(_C.pointer(ans), _C.c_char_p)
        item = Dbi._dbi_item(buflen, code, pointer)
        _exc.checkStatus(_TreeShr._TreeSetDbi(self.ctx, _C.byref(item)))

    name = expt = treename = Dbi._dbiProp(Dbi.NAME,             "Tree name")
    shotid = Dbi._dbiProp(Dbi.SHOTID,           "Shot number of tree")
    modified = Dbi._dbiProp(
        Dbi.MODIFIED,         "True if open for edit and modifications made to tree structure.")
    open_for_edit = Dbi._dbiProp(
        Dbi.OPEN_FOR_EDIT,    "True if tree is opened for edit")
    index = Dbi._dbiProp(
        Dbi.INDEX,            "Index of tree to use for subsequent information requests")
    number_opened = Dbi._dbiProp(
        Dbi.NUMBER_OPENED,    "Number of open trees on tree stack")
    max_open = Dbi._dbiProp(
        Dbi.MAX_OPEN,         "Max number of trees to keep open on stack")
    open_readonly = Dbi._dbiProp(
        Dbi.OPEN_READONLY,    "True if tree is open readonly")
    versions_in_model = Dbi._dbiProp(
        Dbi.VERSIONS_IN_MODEL, "Support versioning of data in model.", True)
    versions_in_pulse = Dbi._dbiProp(
        Dbi.VERSIONS_IN_PULSE, "Support versioning of data in pulse.", True)
    dispatch_table = Dbi._dbiProp(
        Dbi.DISPATCH_TABLE,   "True if dispatch table is built")

    @property
    def default(self):
        "current default node position in tree (settable)"
        return self.getDefault()

    @default.setter
    def default(self, treenode):
        self.setDefault(treenode)

    @property
    def top(self):  # compatibility
        "Tree root"
        return TreeNode(0, self)

    def __getattr__(self, name):
        """Support for referencing an immediate child or
        member of current default node of the tree by
        specifying an uppercase property. For example:

        t = Tree('mytree',shot)
        node = t.NODENAME

        If the tree has a top level child or member with
        the name "NODENAME" t.NODENAME will return a
        TreeNode instance."""
        if name == "tree":
            return self.name
        if name == "shot":
            return self.shotid
        try:
            return _getNodeByAttr(self, name)
        except _exc.TreeNNF:
            raise AttributeError('No such attribute: '+name)

    @staticmethod
    def usingPrivateCtx():
        return bool(_TreeShr.TreeUsingPrivateCtx())

    @staticmethod
    def usePrivateCtx(on=True):
        if on:
            val = _C.c_int32(1)
        else:
            val = _C.c_int32(0)
        return _TreeShr.TreeUsePrivateCtx(val)

    def __deepcopy__(self, memo):
        return self

    def __eq__(self, obj):
        if isinstance(obj, (Tree,)):
            return self.ctx.value == obj.ctx.value
        return False

    def __ne__(self, obj): return not self.__eq__(obj)

    def __repr__(self):
        """Return representation
        @return: String representation of open tree
        @rtype: str
        """
        try:
            if self.open_for_edit:
                mode = "Edit"
            elif self.open_readonly:
                mode = "Readonly"
            else:
                mode = "Normal"
        except _exc.TreeNOT_OPEN:
            if self.public:
                return 'Tree("?",?,"Closed")'
            mode = "Closed"
        return 'Tree("%s",%d,"%s")' % (self.tree, self.shot, mode)

    __str__ = __repr__

    def addDevice(self, nodename, model):
        """Add a device to the tree of the specified device model type.
        @param nodename: Absolute or relative path specification of the head node of the device.
                         All ancestors of node must exist.
        @type nodename: str
        @param model: Model name of the device being added.
        @type model: str
        @return: Head node of device added
        @rtype: TreeNode
        """
        nid = _C.c_int32(0)
        with self._lock:
            _exc.checkStatus(
                _TreeShr._TreeAddConglom(self.ctx,
                                         _ver.tobytes(nodename),
                                         _ver.tobytes(model),
                                         _C.byref(nid)))
        return TreeNode(nid.value, self)

    def addNode(self, nodename, usage='ANY'):
        """Add a node to the tree. Tree must be in edit mode.
        @param nodename: Absolute or relative path specification of new node. All ancestors of node must exist.
        @type nodename: str
        @param usage: Usage of node.
        @type usage: str
        @return: Node created.
        @rtype: TreeNode
        """
        nid = _C.c_int32(0)
        try:
            usage_idx = _usage_table[usage.upper()]
        except KeyError:
            raise UsageError(usage)
        usagenum = 1 if usage_idx == 11 else usage_idx
        with self._lock:
            _exc.checkStatus(
                _TreeShr._TreeAddNode(self.ctx,
                                      _ver.tobytes(nodename),
                                      _C.byref(nid),
                                      _C.c_int32(usagenum)))
            if usage_idx == 11:
                _exc.checkStatus(
                    _TreeShr._TreeSetSubtree(self.ctx, nid))
        return TreeNode(nid.value, self)

    def createPulse(self, shot):
        """Create pulse.
        @param shot: Shot number to create
        @type shot: int
        @rtype: None
        """
        _exc.checkStatus(
            _TreeShr._TreeCreatePulseFile(self.ctx,
                                          _C.c_int32(int(shot)),
                                          _C.c_int32(0),
                                          _C.c_void_p(0)))

    def deleteNode(self, wild):
        """Delete nodes (and all their descendants) from the tree. Note: If node is a member of a device,
        all nodes from that device are also deleted as well as any descendants that they might have.
        @param wild: Wildcard path speficier of nodes to delete from tree.
        @type wild: str
        @rtype: None
        """
        with self._lock:
            first = True
            nodes = self.getNodeWild(wild)
            for node in nodes:
                if first:
                    reset = _C.c_int32(1)
                    first = False
                else:
                    reset = _C.c_int32(0)
                _exc.checkStatus(
                    _TreeShr._TreeDeleteNodeInitialize(self.ctx,
                                                       node._nid,
                                                       0,
                                                       reset))
            _exc.checkStatus(
                _TreeShr._TreeDeleteNodeExecute(self.ctx))

    def deletePulse(self, shot):
        """Delete pulse.
        @param shot: Shot number to delete
        @type shot: int
        @rtype: None
        """
        _exc.checkStatus(
            _TreeShr._TreeDeletePulseFile(self.ctx,
                                          _C.c_int32(int(shot)),
                                          _C.c_int32(1)))

    def dir(self):
        """Return current default dir"""
        self.default.dir()

    def __dir__(self):
        """used for tab completion"""
        return [str(n.node_name) for n in self.top.descendants]+_ver.superdir(Tree, self)

    def findTagsIter(self, wild):
        """An iterator for the tagnames from a tree given a wildcard specification.
        @param wild: wildcard spec.
        @type wild: str
        @return: iterator of tagnames (strings) that match the wildcard specification
        @rtype: iterator
        """
        nid = _C.c_int32(0)
        tagctx = _C.c_void_p(0)
        _TreeShr._TreeFindTagWild.restype = _C.c_char_p
        try:
            while True:
                tag_ptr = _TreeShr._TreeFindTagWild(self.ctx,
                                                    _C.c_char_p(
                                                        _ver.tobytes(wild)),
                                                    _C.byref(nid),
                                                    _C.byref(tagctx))
                if tag_ptr is None:
                    break
                yield tag_ptr.rstrip()
        except GeneratorExit:
            pass
        _TreeShr.TreeFindTagEnd(_C.byref(tagctx))

    def findTags(self, wild):
        """Find tags matching wildcard expression
        @param wild: wildcard string to match tagnames.
        @type wild: str
        @return: Array of tag names matching wildcard expression
        @rtype: ndarray
        """
        return tuple(self.findTagsIter(wild))

    @classmethodX
    def getCurrent(self, tree=None):
        """Return current shot for specificed tree
        @param tree: Name of tree
        @type tree: str
        @return: Current shot number for the specified tree
        @rtype: int
        """
        if isinstance(self, (Tree,)):
            tree = self.tree
        shot = _TreeShr.TreeGetCurrentShotId(_ver.tobytes(tree))
        if shot == 0:
            raise _exc.TreeNOCURRENT()
        return shot

    def getDefault(self):
        """Return current default TreeNode
        @return: Current default node
        @rtype: TreeNode
        """
        nid = _C.c_int32(0)
        _exc.checkStatus(
            _TreeShr._TreeGetDefaultNid(self.ctx,
                                        _C.byref(nid)))
        return TreeNode(nid.value, self)

    def getNode(self, name):
        """Locate node in tree. Returns TreeNode if found. Use double backslashes in node names.
        @param name: Name of node. Absolute or relative path. No wildcards.
        @type name: str
        @return: Node if found
        @rtype: TreeNode
        """

        if isinstance(name, (int, _scr.Int32)):
            ans = TreeNode(name, self)
        else:
            nid = _C.c_int32(0)
            _exc.checkStatus(
                _TreeShr._TreeFindNode(self.ctx,
                                       _ver.tobytes(str(name)),
                                       _C.byref(nid)))
            return TreeNode(int(nid.value), self)
        return ans

    def _getNodeWildIter(self, name, *usage):
        if len(usage) == 0:
            usage_mask = 0xFFFF
        else:
            try:
                usage_mask = 0
                for u in usage:
                    usage_mask |= 1 << _usage_table[u.upper()]
            except KeyError:
                raise UsageError(u)

        nid = _C.c_int32(0)
        ctx = _C.c_void_p(0)
        try:
            while _TreeShr._TreeFindNodeWild(self.ctx,
                                             _ver.tobytes(name),
                                             _C.byref(nid),
                                             _C.byref(ctx),
                                             _C.c_int32(usage_mask)) & 1 != 0:
                yield nid.value
        except GeneratorExit:
            pass
        _TreeShr._TreeFindNodeEnd(self.ctx, _C.pointer(ctx))

    def getNodeWildIter(self, name, *usage):
        """An iterator for the nodes in a tree given a wildcard specification.
        @param name: Node name. May include wildcards.
        @type name: str
        @param usage: Optional list of node usages (i.e. "Numeric","Signal",...). Reduces return set by including only nodes with these usages.
        @type usage: str
        @return: iterator of TreeNodes that match the wildcard and usage specifications
        @rtype: iterator
        """
        for nid in self._getNodeWildIter(name, *usage):
            yield TreeNode(nid, self)

    def getNodeWild(self, name, *usage):
        """Find nodes in tree using a wildcard specification. Returns TreeNodeArray if nodes found.
        @param name: Node name. May include wildcards.
        @type name: str
        @param usage: Optional list of node usages (i.e. "Numeric","Signal",...). Reduces return set by including only nodes with these usages.
        @type usage: str
        @return: TreeNodeArray of nodes matching the wildcard path specification and usage types.
        @rtype: TreeNodeArray
        """
        return TreeNodeArray([nid for nid in self._getNodeWildIter(name, *usage)], self)

    @classmethodX
    def getTimeContext(self):
        """Get time context for retrieving segmented records (begin,end,delta)
        @rtype: tuple
        """
        begin = _dsc.DescriptorXD()
        end = _dsc.DescriptorXD()
        delta = _dsc.DescriptorXD()
        if isinstance(self, (Tree,)):
            begin._setTree(self)
            end._setTree(self)
            delta._setTree(self)
            _exc.checkStatus(_TreeShr._TreeGetTimeContext(
                self.ctx, begin.ref, end.ref, delta.ref))
        else:
            _exc.checkStatus(_TreeShr.TreeGetTimeContext(
                begin.ref, end.ref, delta.ref))
        return (begin.value, end.value, delta.value)

    @staticmethod
    def getVersionDate():
        """Get date used for retrieving versions
        @return: Reference date for retrieving data is versions enabled
        @rtype: str
        """
        dt = _C.c_ulonglong(0)
        _exc.checkStatus(_TreeShr.TreeGetViewDate(dt))
        return _scr.Uint64(dt.value).date

    @classmethodX
    def incrementCurrent(self, tree=None, inc=1):
        """Increments current shot for specified tree by inc
        @param tree: Name of tree
        @type tree: str
        @param inc: Increment (default: 1)
        @rtype int
        """
        if isinstance(self, (Tree,)):  # instancemethod: args shifted by one
            if tree is not None:
                inc = tree
            tree = self.tree
        shot = Tree.getCurrent(tree)+inc
        Tree.setCurrent(tree, shot)
        return shot

    def isModified(self):
        """Check to see if tree is open for edit and has been modified
        @return: True if tree structure has been modified.
        @rtype: bool
        """
        return self.modified

    def isOpenForEdit(self):
        """Check to see if tree is open for edit
        @return: True if tree is open for edit
        @rtype: bool
        """
        return self.open_for_edit

    def isReadOnly(self):
        """Check to see if tree was opened readonly
        @return: True if tree is open readonly
        @rtype: bool
        """
        return self.open_readonly

    def removeTag(self, tag):
        """Remove a tagname from the tree
        @param tag: Tagname to remove.
        @type tag: str
        @rtype: None
        """
        with self._lock:
            _exc.checkStatus(
                _TreeShr._TreeRemoveTag(self.ctx,
                                        _C.c_char_p(_ver.tobytes(tag))))

    @classmethodX
    def setCurrent(self, tree=None, shot=None):
        """Set current shot for specified tree
        @param tree: Name of tree
        @type tree: str
        @param shot: Shot number
        @type shot: int
        @rtype None
        """
        if isinstance(self, (Tree,)):  # instancemethod: args shifted by one
            shot = self.shot if tree is None else tree
            tree = self.tree
        _exc.checkStatus(
            _TreeShr.TreeSetCurrentShotId(_C.c_char_p(_ver.tobytes(tree)),
                                          _C.c_int32(int(shot))))

    def setDefault(self, node):
        """Set current default TreeNode.
        @param node: Node to make current default. Relative node paths will use the current default when resolving node lookups.
        @type node: TreeNode
        @return: Previous default node
        @rtype: TreeNode
        """
        old = self.default
        if not isinstance(node, TreeNode):
            raise TypeError('default node must be a TreeNode')
        if not node.ctx.value == self.ctx.value:
            raise TypeError('TreeNode must be in same tree')
        _exc.checkStatus(
            _TreeShr._TreeSetDefaultNid(self.ctx,
                                        node._nid))
        return old

    @classmethodX
    def setTimeContext(self, begin=None, end=None, delta=None):
        """Set time context for retrieving segmented records
        @param begin: Time value for beginning of segment.
        @type begin: str, Uint64, Float32 or Float64
        @param end: Ending time value for segment of data
        @type end: str, Uint64, Float32 or Float64
        @param delta: Delta time for sampling segment
        @type delta: Uint64, Float32 or Float64
        @rtype: None
        """
        def prepValue(value):
            if value is None:
                return value, _C.c_void_p(0)
            value = _dat.Data(value)
            return value, _dat.Data.byref(value)

        begin, begin_p = prepValue(begin)
        end,  end_p = prepValue(end)
        delta, delta_p = prepValue(delta)
        if isinstance(self, (Tree,)):
            _exc.checkStatus(_TreeShr._TreeSetTimeContext(
                self.ctx, begin_p, end_p, delta_p))
        else:
            _exc.checkStatus(_TreeShr.TreeSetTimeContext(
                begin_p, end_p, delta_p))

    @staticmethod
    def setVersionDate(date):
        """Set date for retrieving versions if versioning is enabled in tree.
        @param date: Reference date for data retrieval. Must be specified in the format: 'mmm-dd-yyyy hh:mm:ss' or 'now','today'
        or 'yesterday'.
        @type date: str
        @rtype: None
        """
        _exc.checkStatus(
            _TreeShr.TreeSetViewDate(_C.byref(_C.c_int64(_mds.DateToQuad(date).data()))))

    def setVersionsInModel(self, flag):
        """Enable/Disable versions in model
        @param flag: True or False. True enables versions
        @type flag: bool
        @rtype: None
        """
        self.versions_in_model = bool(flag)

    def setVersionsInPulse(self, flag):
        """Enable/Disable versions in pulse
        @param flag: True or False. True enabled versions
        @type flag: bool
        @rtype: None
        """
        self.versions_in_pulse = bool(flag)

    def versionsInModelEnabled(self):
        """Check to see if versions in the model are enabled
        @return: True if versions in model is enabled
        @rtype: bool
        """
        return self.versions_in_model

    def versionsInPulseEnabled(self):
        """Check to see if versions in the pulse are enabled
        @return: True if versions in pulse is enabled
        @rtype: bool
        """
        return self.versions_in_pulse

    def write(self):
        """Write out edited tree.
        @rtype: None
        """
        with self._lock:
            _exc.checkStatus(_TreeShr._TreeWriteTree(self.pctx, 0, 0))

    def tcl(self, cmd, *args, **kwargs):
        """tree specific tcl command"""
        kwargs['tree'] = self
        return _dcl.tcl(cmd, *args, **kwargs)

    def tdiCompile(self, *args, **kwargs):
        """Compile a TDI expression. Format: tdiCompile('expression-string',(arg1,...))"""
        kwargs['tree'] = self
        return _dat.TdiCompile(*args, **kwargs)

    def tdiExecute(self, *args, **kwargs):
        """Compile and execute a TDI expression. Format: tdiExecute('expression-string',(arg1,...))"""
        kwargs['tree'] = self
        return _dat.TdiExecute(*args, **kwargs)

    def tdiEvaluate(self, arg, **kwargs):
        """Evaluate and functions. Format: tdiEvaluate(data)"""
        kwargs['tree'] = self
        return _dat.TdiEvaluate(arg, **kwargs)

    def tdiData(self, arg, **kwargs):
        """Return primitive data type. Format: tdiData(value)"""
        kwargs['tree'] = self.tree
        return _dat.TdiData(arg, **kwargs)


# HINT: TreeNode begin  (maybe subclass of _scr.Int32 some day)
class TreeNode(_dat.TreeRef, _dat.Data):
    """Class to represent an MDSplus node reference (nid).
    @ivar nid: node index of this node.
    @type nid: int
    @ivar tree: Tree instance that this node belongs to.
    @type tree: Tree
    """
    dtype_id = 192
    _units = None
    _error = None
    _help = None
    _validation = None
    _nid = None
    _path = None

    @property
    def ctx(self): return _C.c_void_p(_TreeShr.TreeDbid()
                                      ) if self.tree is None else self.tree.ctx

    @property
    def _lock(self): return self.tree._lock

    def __new__(cls, nid, tree=None, head=None, *a, **kw):
        """Create class instance. Initialize part_dict class attribute if necessary.
        @param nid: Node of device
        @type nid: TreeNode
        @return: Instance of the device subclass
        @rtype: Device subclass instance
        """
        node = super(TreeNode, cls).__new__(cls)
        head = nid._head if isinstance(nid, TreeNode) else head
        if not isinstance(head, (Device,)) and type(node) is TreeNode:
            TreeNode.__init__(node, nid, tree, head, *a, **kw)
            try:
                if str(node.usage) == "DEVICE":
                    return node.record.getDevice(node, head=0)
            except(_exc.TreeNODATA, _exc.DevNOT_A_PYDEVICE, _exc.DevPYDEVICE_NOT_FOUND):
                pass
        return node

    def __init__(self, nid, tree=None, head=None, *a, **kw):
        """Initialze TreeNode
        @param nid: Index of the node in the tree.
        @type nid: int
        @param tree: Tree associated with this node
        @type tree: Tree
        """
        if self is nid:
            return
        if isinstance(nid, TreeNode):
            self._nid = nid._nid
            self.tree = nid.tree
            self._head = nid._head
        else:
            self._nid = None if nid is None else _C.c_int32(int(nid))
            if isinstance(tree, (Tree,)):
                self.tree = tree
            elif isinstance(tree, (tuple,)):
                self.tree = Tree(*tree)
            else:
                self.tree = Tree()
            self._head = head

    def copy(self, mode='NORMAL'):
        """returns the node with a local private instance of the tree opend in specified mode
        @param mode: Optional mode, one of 'Normal','Edit','New','Readonly'
        @type mode: str
        @rtype: TreeNode
        """
        return self.__class__(self.nid, self.tree.copy(mode))

    _head = None

    @property
    def head(self):
        if self._head is None:
            if self.conglomerate_elt > 0:
                nids = self.conglomerate_nids.nid_number
                head_nid = int(nids[0])
                if head_nid == self._nid:
                    self._original_part_name = ""
                    self._head = 0
                else:
                    self._head = TreeNode(head_nid, self.tree, 0)
            else:
                self._head = -1
        if isinstance(self._head, (TreeNode,)):
            return self._head
        return self

    _original_part_name = None

    def ORIGINAL_PART_NAME(self):
        """Method to return the original part name.
        Will return blank string if part_name class attribute not defined or node used to create instance is the head node or past the end of part_names tuple.
        @return: Part name of this node
        @rtype: str
        """
        if self.head is self or not isinstance(self.head, Device):
            return ""
        if self._original_part_name is None:
            try:
                self._original_part_name = self.head.__class__.parts[self.nid -
                                                                     self.head.nid-1]['path']
            except:
                self._original_part_name = ""
        return self._original_part_name
    PART_NAME = ORIGINAL_PART_NAME

    @property
    def _descriptor(self):
        """Return a MDSplus descriptor"""
        d = _dsc.DescriptorS()
        d.length = 4
        d.dtype = self.dtype_id
        d.pointer = _C.cast(_C.pointer(self._nid), _C.c_void_p)
        d.tree = self.tree
        return _cmp.Compound._descriptorWithProps(self, d)

    def __eq__(self, obj):
        if isinstance(obj, (TreeNode,)):
            return self.tree == obj.tree and self.nid == obj.nid
        return False

    def __ne__(self, obj): return not self.__eq__(obj)

    ###################################
    # Node Properties
    ###################################

    def _getNci(self, info):
        """Return nci data"""
        code, ctype, buflen, rtype = info
        if ctype is _C.c_char_p:
            ans = ctype((b' ')*buflen)
            pointer = ans
        else:
            ans = ctype()
            pointer = _C.cast(_C.pointer(ans), _C.c_char_p)
        item = Nci._nci_item(buflen, code, pointer)
        _exc.checkStatus(_TreeShr._TreeGetNci(
            self.ctx, self._nid, _C.byref(item)))
        retlen = item.retlen.contents.value

        if rtype is str:
            return _ver.tostr(ans.value[0:retlen].rstrip())
        if rtype is None:
            if ctype is _C.c_int32:
                return TreeNode(int(ans.value), self.tree)
            return TreeNodeArray([int(ans[i]) for i in _ver.xrange(retlen//4)], self.tree)
        return rtype(ans.value)

    def _setNci(self, code, setting):
        pointer = _C.cast(_C.pointer(_C.c_uint32(setting)), _C.c_char_p)
        _exc.checkStatus(_TreeShr._TreeSetNci(
            self.ctx, self._nid, _C.byref(Nci._nci_item(0, code, pointer))))

    def _setNciFlag(self, mask, setting):
        value = 1 if setting else 2
        pointer = _C.cast(_C.pointer(_C.c_uint32(mask)), _C.c_char_p)
        item = Nci._nci_item(0, value, pointer)
        _exc.checkStatus(_TreeShr._TreeSetNci(
            self.ctx, self._nid, _C.byref(item)))

    cached = Nci._nciProp(Flags.CACHED, "True if data is cached")
    compress_segments = Flags._nciFlag(
        Flags.COMPRESS_SEGMENTS, "should segments be compressed")
    compressible = Nci._nciProp(
        Flags.COMPRESSIBLE, "is the data stored in this node compressible")
    essential = Flags._nciFlag(
        Flags.ESSENTIAL, "essential action defined in this node")
    do_not_compress = Flags._nciFlag(
        Flags.DO_NOT_COMPRESS, "is this node set to disable any compression of data stored in it")
    compress_on_put = Flags._nciFlag(
        Flags.COMPRESS_ON_PUT, "should data be compressed when stored in this node")
    include_in_pulse = Flags._nciFlag(
        Flags.INCLUDE_IN_PULSE, "include subtree in pulse")
    nid_reference = Nci._nciProp(
        Flags.NID_REFERENCE, "node data contains nid references")
    no_write_model = Flags._nciFlag(
        Flags.NO_WRITE_MODEL, "is storing data in this node disabled if model tree")
    no_write_shot = Flags._nciFlag(
        Flags.NO_WRITE_SHOT, "is storing data in this node disabled if not model tree")
    parent_state = Nci._nciProp(Flags.PARENT_STATE, "is parent disabled")
    path_reference = Nci._nciProp(
        Flags.PATH_REFERENCE, "node data contains path references")
    segmented = Nci._nciProp(Flags.SEGMENTED, "is data segmented")
    setup_information = setup = Nci._nciProp(
        Flags.SETUP_INFORMATION, "was this data present in the model")
    state = Flags._nciFlag(
        Flags.STATE, "Use on property instead. on/off state of this node. False=on,True=off.")
    versions = Nci._nciProp(Flags.VERSIONS, "does the data contain versions")
    write_once = Flags._nciFlag(Flags.WRITE_ONCE, "is no write once")

    brother = Nci._nciProp(Nci.BROTHER, "brother node of this node")
    child = Nci._nciProp(Nci.CHILD, "child node of this node")
    class_str = Nci._nciProp(
        Nci.CLASS_STR, "class name of the data stored in this node")
    conglomerate_elt = Nci._nciProp(
        Nci.CONGLOMERATE_ELT, "what element of a conglomerate is this node")
    conglomerate_nids = Nci._nciProp(
        Nci.CONGLOMERATE_NIDS, "what are the nodes of the conglomerate this node belongs to")
    data_in_nci = Nci._nciProp(
        Nci.DATA_IN_NCI, "is the data of this node stored in its nci")
    depth = Nci._nciProp(
        Nci.DEPTH, "what is the depth of this node in the tree structure")
    dtype = Nci._nciProp(
        Nci.DTYPE, "the numeric value of the data type stored in this node")
    dtype_str = Nci._nciProp(
        Nci.DTYPE_STR, "the name of the data type stored in this node")
    error_on_put = Nci._nciProp(
        Nci.ERROR_ON_PUT, "was there an error storing data for this node")
    fullpath = Nci._nciProp(Nci.FULLPATH, "full node path")
    get_flags = Nci._nciProp(Nci.GET_FLAGS, "numeric flags mask for this node")
    length = Nci._nciProp(
        Nci.LENGTH, "length of data stored in this node (uncompressed)")
    mclass = _class = Nci._nciProp(
        Nci.CLASS, "class of the data stored in this node")
    member = Nci._nciProp(
        Nci.MEMBER, "first member immediate descendant of this node")
    minpath = Nci._nciProp(
        Nci.MINPATH, "minimum path string for this node based on current default node")
    node_name = name = Nci._nciProp(Nci.NODE_NAME, "node name")
    number_of_children = Nci._nciProp(
        Nci.NUMBER_OF_CHILDREN, "number of children")
    number_of_elts = Nci._nciProp(
        Nci.NUMBER_OF_ELTS, "number of nodes in a conglomerate")
    number_of_members = Nci._nciProp(
        Nci.NUMBER_OF_MEMBERS, "number of members")
    original_part_name = Nci._nciProp(
        Nci.ORIGINAL_PART_NAME, "original part name of this node")
    owner_id = Nci._nciProp(
        Nci.OWNER_ID, "id of the last person to write to this node")
    parent = Nci._nciProp(Nci.PARENT, "parent node of this node")
    parent_relationship = Nci._nciProp(
        Nci.PARENT_RELATIONSHIP, "parent relationship")
    rfa = Nci._nciProp(Nci.RFA, "data offset in datafile")
    rlength = Nci._nciProp(Nci.RLENGTH, "length of data in node")
    status = Nci._nciProp(Nci.STATUS, "status of action execution")
    time_inserted = Nci._nciProp(
        Nci.TIME_INSERTED, "64-bit timestamp when data was stored")
    usage_str = Nci._nciProp(
        Nci.USAGE_STR, "formal name of the usage of this node")
    compression_method = Nci._nciProp(
        Nci.COMPRESSION_METHOD, "numerical code for the compression method to use for this node")
    compression_method_str = Nci._nciProp(
        Nci.COMPRESSION_METHOD_STR, "name of the compression algorithm to use for this node")
    
    __children_nids = Nci._nciProp(Nci.CHILDREN_NIDS)
    __member_nids = Nci._nciProp(Nci.MEMBER_NIDS)

    @property
    def children_nids(self):
        """children nodes of this node"""
        try:
            return self.__children_nids
        except _exc.TreeNNF:
            return TreeNodeArray([], self.tree)

    @property
    def member_nids(self):
        """all member immediate descendants of this node"""
        try:
            return self.__member_nids
        except _exc.TreeNNF:
            return TreeNodeArray([], tree=self.tree)

    @property
    def descendants(self):
        "Get all the immediate descendants of this node"
        return self.children_nids+self.member_nids

    @property
    def number_of_descendants(self):
        "Number of immediate descendants of this node."
        return self.number_of_children+self.number_of_members

    @property
    def disabled(self):
        "is this node disabled (settable)"
        return self.isDisabled()

    @disabled.setter
    def disabled(self, value): self.setOn(not value)

    @property
    def is_child(self):
        "is this a child node?"
        return self.isChild()

    @property
    def is_member(self):
        "is this a member node?"
        return self.isMember()

    @property
    def local_path(self):
        "Return path relative to top of tree this node is part of."
        return self.getLocalPath()

    @property
    def local_tree(self):
        "Return name of the tree this node is part of."
        return self.getLocalTree()

    @property
    def nid_number(self):
        "the node id of the node"
        if self._nid is None:
            return None
        else:
            return _scr.Int32(self._nid.value)

    @property
    def nid(self):
        "the node id of the node"
        if self._nid is None:
            return None
        else:
            return self._nid.value

    @nid.setter
    def nid(self, value):
        if value is None:
            self._nid = None
        else:
            self._nid = _C.c_int32(int(value))

    @property
    def on(self):
        "Is this node turned on (settable)"
        return self.isOn()

    @on.setter
    def on(self, value): self.setOn(value)

    @property
    def parent_disabled(self):
        return self.isParentDisabled()

    @parent_disabled.setter
    def parent_disabled(self, value): self.setParentOn(not value)

    @property
    def parent_on(self):
        return self.isParentOn()

    @parent_on.setter
    def parent_on(self, value):  self.setParentOn(value)

    @property
    def path(self):
        """path to this node"""
        try:
            self._path = self._getNci(Nci.PATH)
        except _exc.TreeNOT_OPEN:
            return '%s /*tree closed*/' % self._path
        return self._path

    @property
    def record(self):
        "Data contents of node (settable)"
        return self.getRecord()

    @record.setter
    def record(self, value):
        self.putData(value)

    def data(self, *altvalue):
        return self.record.data(*altvalue)

    @property
    def subtree(self):
        "Is this node a subtree reference. (settable)"
        return str(self.usage) == "SUBTREE"

    @subtree.setter
    def subtree(self, value): self.setSubtree(value)

    @property
    def tags(self):
        "All tags defined for this node. (settable - note existing tags replace by new tags"
        return self.getTags()

    @tags.setter
    def tags(self, names): self.addTags(names, replace=True)

    @property
    def tag(self):
        "tags defined for this node (settable to add a tag)."
        return self.tags

    @tag.setter
    def tag(self, names): self.addTags(names, replace=False)

    @property
    def usage(self):
        "Usage of this node."
        return _scr.String(str(self.usage_str)[10:])

    @usage.setter
    def usage(self, usage): self.setUsage(usage)

    @property
    def compression_method(self):
        "compression method to use for this node."
        return _scr.String(str(self.compression_method_str))

    @compression_method.setter
    def compression_method(self, compression_method): 
        self.setCompressionMethod(compression_method) 

    ########################################
    # End of Node Properties
    ########################################

    def __deepcopy__(self, dummy):
        return self

    def __hasBadTreeReferences__(self, tree):
        return self.tree != tree

    def __fixTreeReferences__(self, tree):
        if self.tree == None:
            self.tree = tree
        if (self.nid >> 24) != 0:
            return TreePath(str(self), tree)
        else:
            relpath = str(self.fullpath)
            relpath = relpath[relpath.find('::TOP')+5:]
            path = '\\%s::TOP%s' % (tree.tree, relpath)
            try:
                ans = tree.getNode(str(self))
            except _exc.TreeNNF:
                ans = TreePath(path, tree)
            return ans

    def __getattr__(self, name):
        """ Return a member or child node for
            node.abc if abc is uppercase and
            matches the name of an immediate
            ancester to the node.
        """
        # if name=='tree':
        #    return Tree()
        if (name.startswith('__') and
            not name.endswith('__') and
                name[2:].upper() == name[2:]):
            return self.getExtendedAttribute(name[2:])
        try:
            return _getNodeByAttr(self, name)
        except (_exc.TreeNNF, _exc.TreePARSEERR):
            pass
        return super(TreeNode, self).__getattr__(name)
        # if name=='length':
        #    raise AttributeError
        # if self.length>0:
        #    return self.record.__getattr__(name)
        #raise AttributeError('No such attribute: '+name)

    def __setattr__(self, name, value):
        """ Enables the setting of node attributes using
            then format:
            node.__MYATTRIBUTE=value
        """
        if (name.startswith('__') and
            not name.endswith('__') and
                name[2:].upper() == name[2:]):
            self.setExtendedAttribute(name[2:], value)
        else:
            super(TreeNode, self).__setattr__(name, value)

    def __str__(self):
        """Convert TreeNode to string."""
        if self._nid is None:
            return "NODEREF(*)"
        try:
            return str(self.path)
        except:
            return "NODEREF(%d)" % (self._nid.value,)

    def addDevice(self, name, model):
        """Add device descendant.
        @param name: Node name of device. 1-12 characters, no path delimiters
        @type name: str
        @param model: Type of device to add
        @type model: str
        @return: Head node of device
        @rtype: TreeNode
        """
        if name.find(':') >= 0 or name.find('.') >= 0:
            raise TreeNodeException(
                "Invalid node name, do not include path delimiters in nodename")
        return self.tree.addDevice(self.fullpath+":"+name.upper(), model)

    def addNode(self, name, usage='ANY'):
        """Add node
        @param name: Node name of new node to add
        @type name: str
        @param usage: Usage of the new node. If omitted set to ANY.
        @type usage: str
        @return: New now added
        @rtype: TreeNode
        """
        try:
            usagenum = _usage_table[usage.upper()]
        except KeyError:
            raise UsageError(usage)
        name = str(name).upper()
        if name[0] == ':' or name[0] == '.':
            name = str(self.fullpath)+name
        elif name[0] != "\\":
            if usagenum == 1 or usagenum == -1:
                name = str(self.fullpath)+"."+name
            else:
                name = self.fullpath+":"+name
        return self.tree.addNode(name, usage)

    def addTags(self, names, replace=False):
        """Add tag or list of tags to a node.
        Use replace=True if any existing tags should be removed.
        Tag names must be either a str or a list or tuple
        containing str instances."""
        if isinstance(names, _ver.basestring):
            names = (names,)
        else:
            try:
                names = tuple(names)
                for name in names:
                    if not isinstance(name, _ver.basestring):
                        raise TypeError
            except TypeError:
                raise TypeError(
                    "Tag names must be a string or list of strings")
        if replace:
            for name in self.tags:
                self.removeTag(name)
        for name in names:
            self.addTag(name)

    def addTag(self, tag):
        """Add a tagname to this node
        @param tag: tagname for this node
        @type tag: str
        @rtype: None
        """
        _exc.checkStatus(
            _TreeShr._TreeAddTag(self.ctx,
                                 self._nid,
                                 _C.c_char_p(str.encode(tag))))

    def beginSegment(self, start, end, dim, array, idx=-1):
        """Begin a record segment, rows_filled = 0
        @param start: Index of first row of data
        @type start: Data
        @param end: Index of last row of data
        @type end: Data
        @param dim: Dimension information of segment
        @type dim: Dimension
        @param array: Initial data array. Defines shape of segment
        @type array: Array
        @rtype: None
        """
        start, end, dim, array = map(_dat.Data, (start, end, dim, array))
        _exc.checkStatus(
            _TreeShr._TreeBeginSegment(self.ctx,
                                       self._nid,
                                       _dat.Data.byref(start),
                                       _dat.Data.byref(end),
                                       _dat.Data.byref(dim),
                                       _dat.Data.byref(array),
                                       _C.c_int32(int(idx))))

    def beginSegmentResampled(self,start,end,dim,array,resNode,resFactor,idx=-1):
        """Begin a resampled record segment, using an average, rows_filled = 0
        @param start: Index of first row of data
        @type start: Data
        @param end: Index of last row of data
        @type end: Data
        @param dim: Dimension information of segment
        @type dim: Dimension
        @param array: Initial data array. Defines shape of segment
        @type array: Array
        @param resNode: TreeNode of the node to write the resampled data into
        @type resNode: TreeNode
        @param resFactor: Number of samples to resample together
        @type resFactor: Data
        @rtype: None
        """
        start,end,dim,array = map(_dat.Data,(start,end,dim,array))
        _exc.checkStatus(
            _TreeShr._TreeBeginSegmentResampled(self.ctx,
                                                self._nid,
                                                _dat.Data.byref(start),
                                                _dat.Data.byref(end),
                                                _dat.Data.byref(dim),
                                                _dat.Data.byref(array),
                                                _C.c_int32(int(idx)),
                                                resNode._nid,
                                                _C.c_int32(int(resFactor))))

    def beginSegmentMinMax(self,start,end,dim,array,resNode,resFactor,idx=-1):
        """Begin a resampled record segment, using the min and max values
        @param start: Index of first row of data
        @type start: Data
        @param end: Index of last row of data
        @type end: Data
        @param dim: Dimension information of segment
        @type dim: Dimension
        @param array: Initial data array. Defines shape of segment
        @type array: Array
        @param resNode: TreeNode of the node to write the resampled data into
        @type resNode: TreeNode
        @param resFactor: Number of samples to resample together
        @type resFactor: Data
        @rtype: None
        """
        start,end,dim,array = map(_dat.Data,(start,end,dim,array))
        _exc.checkStatus(
            _TreeShr._TreeBeginSegmentMinMax(self.ctx,
                                             self._nid,
                                             _dat.Data.byref(start),
                                             _dat.Data.byref(end),
                                             _dat.Data.byref(dim),
                                             _dat.Data.byref(array),
                                             _C.c_int32(int(idx)),
                                             resNode._nid,
                                             _C.c_int32(int(resFactor))))

    def beginTimestampedSegment(self,array,idx=-1):
        """Allocate space for a timestamped segment
        @param array: Initial data array to define shape of segment
        @type array: Array
        @param idx: Optional segment index. Defaults to -1 meaning next segment.
        @type idx: int
        @rtype: None
        """
        array = _dat.Data(array)
        _exc.checkStatus(
            _TreeShr._TreeBeginTimestampedSegment(self.ctx,
                                                  self._nid,
                                                  _dat.Data.byref(array),
                                                  _C.c_int32(int(idx))))

    def compare(self, value, contents=True):
        """Returns True if this node contains the same data as specified in the value argument
        @param value: Value to compare contents of the node with
        @type value: Data
        @rtype: Bool
        """
        if contents:
            try:
                r = self.record
                if isinstance(r, TreeNode):
                    return r.compare(value, contents=False)
                else:
                    return r.compare(value)
            except _exc.TreeNODATA:
                return (value is None) or (value == _dat.EmptyData)
        else:
            return isinstance(value, TreeNode) and (str(self) == str(value)) and (self.tree == value.tree)

    def containsVersions(self):
        """Return true if this node contains data versions
        @return: True if node contains versions
        @rtype: bool
        """
        return self.versions

    def delete(self):
        """Delete this node from the tree
        @rtype: None
        """
        self.tree.deleteNode(self.fullpath)

    def deleteData(self):
        """Delete data from this node
        @rtype: None
        """
        self.putData(None)

    def dir(self):
        """list descendants"""
        for desc in self.descendants:
            print('%-12s    %s' % (desc.node_name, desc.usage))

    def __dir__(self):
        """used for tab completion"""
        return [str(n.node_name) for n in self.descendants]+_ver.superdir(TreeNode, self)

    def dispatch(self, wait=True):
        """Dispatch an action node
        @rtype: None
        """
        a = self.record
        if not isinstance(a, _cmp.Action):
            raise TreeNodeException(
                "Node does not contain an action description")
        else:
            if wait:
                self.tree.tcl("dispatch/wait %s" % (self.fullpath,), 0, 0, 1)
            else:
                self.tree.tcl("dispatch %s" % (self.fullpath,), 0, 0, 1)

    @property
    def deref(self):
        """ resolves as many node references as possible
            device nodes wil be conserved
        """
        if str(self.usage) == 'DEVICE':
            return self
        try:
            ans = self.record
        except:
            return self
        if isinstance(ans, (_dat.Data, TreeNode)):
            return ans.deref
        return ans

    def doMethod(self, method, *args):
        """Execute method on conglomerate element
        @param method: method name to perform
        @type method: str
        @param args: Optional arguments for method
        @type args: Data or Tuple of Data
        @rtype: None
        """
        arglist = [self.ctx]
        xd = _dsc.DescriptorXD()
        argsobj = [self, _scr.String(method)]
        arglist += list(map(_dat.Data.byref, argsobj))
        arglist.append(len(args))
        if len(args) > 0:
            argsobj = list(map(_dat.Data, args))
            arglist.append(_N.array(map(_dat.Data.byref, argsobj)))
        else:
            arglist.append(_C.c_void_p(0))
        arglist += [xd.ref]
        _exc.checkStatus(_TreeShr._TreeDoMethodA(*arglist))
        return xd._setTree(self.tree).value

    @classmethod
    def fromDescriptor(cls, d):
        return cls(_C.cast(d.pointer, _C.POINTER(_C.c_int32)).contents.value, d.tree)

    def getBrother(self):
        """Return sibling of this node
        @return: Sibling of this node
        @rtype: TreeNode
        """
        return self.brother

    def getChild(self):
        """Return first child of this node.
        @return: Return first child of this node or None if it has no children.
        @rtype: TreeNode
        """
        return self.child

    def getChildren(self):
        """Return TreeNodeArray of children nodes.
        @return: Children of this node
        @rtype: TreeNodeArray
        """
        return self.children_nids

    def getClass(self):
        """Return MDSplus class name of this node
        @return: MDSplus class name of the data stored in this node.
        @rtype: String
        """
        return self.class_str

    def getCompressedLength(self):
        """Return compressed data length of this node
        @return: Compress data length of this node
        @rtype: int
        """
        return self.rlength

    def getConglomerateElt(self):
        """Return index of this node in a conglomerate
        @return: element index of this node in a conglomerate. 0 if not in a conglomerate.
        @rtype: Int32
        """
        return self.conglomerate_elt

    def getConglomerateNodes(self):
        """Return TreeNodeArray of conglomerate elements
        @return: Nodes in this conglomerate.
        @rtype: TreeNodeArray
        """
        return self.conglomerate_nids

    def getRecord(self, *altvalue):
        """Return data
        @return: data stored in this node
        @rtype: Data
        """
        xd = _dsc.DescriptorXD()
        status = _TreeShr._TreeGetRecord(self.ctx, self._nid, xd.ref)
        if (status & 1):
            return xd._setTree(self.tree).value
        elif len(altvalue) == 1 and status == _exc.TreeNODATA.status:
            return altvalue[0]
        else:
            raise _exc.MDSplusException(status)
    getData = getRecord

    def getDataNoRaise(self):
        """Return data
        @return: data stored in this node
        @rtype: Data
        """
        return self.getData(None)

    def getDepth(self):
        """Get depth of this node in the tree
        @return: number of levels between this node and the top of the currently opened tree.
        @rtype: Int32
        """
        return self.depth

    def getDescendants(self):
        """Return TreeNodeArray of first level descendants (children and members).
        @return: First level descendants of this node
        @rtype: TreeNodeArray
        """
        return self.descendants

    def getDtype(self):
        """Return the name of the data type stored in this node
        @return: MDSplus data type name of data stored in this node.
        @rtype: String
        """
        return self.dtype_str

    def getExtendedAttribute(self, name):
        """Get extended attribute of node
        @param name: Name of attribute
        @type name: str
        @return: value of attribute or
                       None if no such attribute or
                       attribute has no data
        @rtype: MDSplus data type
        """
        xd = _dsc.DescriptorXD()
        status = _TreeShr._TreeGetXNci(self.ctx,
                                       self.nid,
                                       _C.c_char_p(_ver.tobytes(
                                           str(name).rstrip().lower())),
                                       xd.ref)
        if (status & 1):
            return xd._setTree(self.tree).value
        else:
            return None

    def getExtendedAttributes(self):
        """Get all extended attributes of a node
        @return: Dictionary of attributes or None if no attributes
        @rtype: dict
        """
        ans = dict()
        attributes = self.getExtendedAttribute("attributenames")
        if attributes is None:
            return None
        for attr in attributes:
            val = self.getExtendedAttribute(str(attr).rstrip())
            if val is not None:
                ans[str(attr).upper().rstrip()] = val
        if len(ans) > 0:
            return ans
        else:
            return None

    def getFullPath(self):
        """Return full path of this node
        @return: full path specification of this node.
        @rtype: String
        """
        return self.fullpath

    def getLength(self):
        """Return uncompressed data length of this node
        @return: Uncompressed data length of this node
        @rtype: int
        """
        return self.length

    def getLocalTree(self):
        """Return tree containing this node
        @return: Name of tree containing this node
        @rtype: str
        """
        top = self
        while top.nid & 0xffffff:
            top = top.parent
        if top.node_name == 'TOP':
            return self.tree.tree
        else:
            return top.node_name

    def getLocalPath(self):
        """Return path relative to top of local tree
        @return: Path relative to top of local tree
        @rtype: str
        """
        path = ''
        top = self
        while top.nid & 0xffffff:
            if top.is_member:
                delim = ':'
            else:
                delim = '.'
            path = delim + top.node_name + path
            top = top.parent
        return path

    def getMember(self):
        """Return first member node
        @return: First member of thie node
        @rtype: TreeNode
        """
        return self.member

    def getMembers(self):
        """Return TreeNodeArray of this nodes members
        @return: members of this node
        @rtype: TreeNodeArray
        """
        return self.member_nids

    def getMinPath(self):
        """Return shortest path string for this node
        @return: shortest path designation depending on the current node default and whether the node has tag names or not.
        @rtype: String
        """
        return self.minpath

    def getNid(self):
        """Return node index
        @return: Internal node index of this node
        @rtype: int
        """
        return self.nid

    def getNode(self, path):
        """Return tree node where path is relative to this node
        @param path: Path relative to this node
        @type path: str
        @return: node matching path
        @rtype: TreeNode
        """
        if isinstance(path, (int, _scr.Int32)):
            ans = TreeNode(path, self.tree)
        else:
            nidout = _C.c_int32(0)
            _exc.checkStatus(
                _TreeShr._TreeFindNodeRelative(self.ctx,
                                               _ver.tobytes(str(path)),
                                               _C.c_int32(self.nid),
                                               _C.byref(nidout)))
            return TreeNode(int(nidout.value), self.tree)
        return ans

    def getNodeName(self):
        """Return node name
        @return: Node name of this node. 1 to 12 characters
        @rtype: String
        """
        return self.node_name

    def getNodeWild(self, path, *usage):
        """Find nodes in tree using a wildcard specification. Returns TreeNodeArray if nodes found.
        @param name: Node name. May include wildcards.
        @type name: str
        @param usage: Optional list of node usages (i.e. "Numeric","Signal",...). Reduces return set by including only nodes with these usages.
        @type usage: str
        @return: TreeNodeArray of nodes matching the wildcard path specification and usage types.
        @rtype: TreeNodeArray
        """
        return TreeNodeArray([nid for nid in self._getNodeWildIter(path, *usage)], self.tree)

    def _getNodeWildIter(self, name, *usage):
        if len(usage) == 0:
            usage_mask = 0xFFFF
        else:
            try:
                usage_mask = 0
                for u in usage:
                    usage_mask |= 1 << _usage_table[u.upper()]
            except KeyError:
                raise UsageError(u)

        nid = _C.c_int32(0)
        ctx = _C.c_void_p(0)
        try:
            while _TreeShr._TreeFindNodeWildRelative(self.ctx,
                                                     _ver.tobytes(name),
                                                     _C.c_int32(self.nid),
                                                     _C.byref(nid),
                                                     _C.byref(ctx),
                                                     _C.c_int32(usage_mask)) & 1 != 0:
                yield nid.value
        except GeneratorExit:
            pass
        _TreeShr._TreeFindNodeEnd(self.ctx, _C.pointer(ctx))

    def getNumChildren(self):
        """Return number of children nodes.
        @return: Number of children
        @rtype: Int32
        """
        return self.number_of_children

    def getNumDescendants(self):
        """Return number of first level descendants (children and members)
        @return: total number of first level descendants of this node
        @rtype: int
        """
        return self.number_of_descendants

    def getNumElts(self):
        """Return number of nodes in this conglomerate
        @return: Number of nodes in this conglomerate or 0 if not in a conglomerate.
        @rtype: Int32
        """
        return self.number_of_elts

    def getNumMembers(self):
        """Return number of members
        @return: number of members
        @rtype: int
        """
        return self.number_of_members

    def getNumSegments(self):
        """return number of segments contained in this node
        @rtype: int
        """
        num = _C.c_int32(0)
        _exc.checkStatus(
            _TreeShr._TreeGetNumSegments(self.ctx,
                                         self._nid,
                                         _C.byref(num)))
        return num.value

    def getOriginalPartName(self):
        """Return the original part name of node in conglomerate
        @return: Original part name of this node when conglomerate was first instantiated.
        @rtype: String
        """
        return self.original_part_name

    def getOwnerId(self):
        """Get id/gid value of account which wrote data to this node
        @return: Return user id of last account used to write data to this node
        @rtype: int
        """
        return self.owner_id

    def getParent(self):
        """Return parent of this node
        @return: Parent of this node
        @rtype: TreeNode
        """
        return self.parent

    def getPath(self):
        """Return path of this node
        @return: Path to this node.
        @rtype: String
        """
        return self.path

    def getSegment(self, idx):
        """Return segment
        @param idx: The index of the segment to return. Indexes start with 0.
        @type idx: int
        @return: Data segment
        @rtype: Signal | None
        """
        val = _dsc.DescriptorXD()._setTree(self.tree)
        dim = _dsc.DescriptorXD()._setTree(self.tree)
        try:
            _exc.checkStatus(
                _TreeShr._TreeGetSegment(self.ctx,
                                         self._nid,
                                         _C.c_int32(int(idx)),
                                         val.ref,
                                         dim.ref))
        except _exc.TreeNOSEGMENTS:
            return None
        try:
            scl = self.getSegmentScale()
        except:
            scl = None
        if scl is None:
            return _cmp.Signal(val.value, None, dim.value)
        return _cmp.Signal(scl, val.value, dim.value)

    def getSegmentDim(self, idx):
        """Return dimension of segment
        @param idx: The index of the segment to return. Indexes start with 0.
        @type idx: int
        @return: Segment dimension
        @rtype: Dimension
        """
        dim = _dsc.DescriptorXD()._setTree(self.tree)
        try:
            _exc.checkStatus(
                _TreeShr._TreeGetSegment(self.ctx,
                                         self._nid,
                                         _C.c_int32(int(idx)),
                                         None,
                                         dim.ref))
        except _exc.TreeNOSEGMENTS:
            return None
        return dim.value

    def getSegmentLimits(self, idx):
        start = _dsc.DescriptorXD()._setTree(self.tree)
        end = _dsc.DescriptorXD()._setTree(self.tree)
        _exc.checkStatus(
            _TreeShr._TreeGetSegmentLimits(self.ctx,
                                           self._nid,
                                           _C.c_int32(int(idx)),
                                           start.ref,
                                           end.ref))
        start, end = start.value, end.value
        _gc.collect()
        if start is not None or end is not None:
            return (start, end)

    def getSegmentList(self, start, end):
        start, end = map(_dat.Data, (start, end))
        xd = _dsc.DescriptorXD()._setTree(self.tree)
        _exc.checkStatus(
            _XTreeShr._XTreeGetSegmentList(self.ctx,
                                           self._nid,
                                           _dat.Data.byref(start),
                                           _dat.Data.byref(end),
                                           xd.ref))
        return xd.value

    def getSegmentScale(self):
        """sets the scale expression of a segmetned Node
        @rtype: expression for the data field; should contain $VALUE
        """
        xd = _dsc.DescriptorXD()._setTree(self.tree)
        _exc.checkStatus(
            _TreeShr._TreeGetSegmentScale(self.ctx,
                                          self._nid,
                                          xd.ref))
        return xd.value

    def getSegmentTimes(self):
        num = _C.c_int32(0)
        start = _dsc.DescriptorXD()._setTree(self.tree)
        end = _dsc.DescriptorXD()._setTree(self.tree)
        _exc.checkStatus(
            _TreeShr._TreeGetSegmentTimesXd(self.ctx,
                                            self._nid,
                                            _C.byref(num),
                                            start.ref,
                                            end.ref))
        return num.value, start.value, end.value

    def getSegmentEnd(self, idx):
        """return end of segment
        @param idx: segment index to query
        @type idx: int
        @rtype: Data
        """
        num = self.getNumSegments()
        if num > 0 and idx < num:
            limits = self.getSegmentLimits(idx)
            if limits is not None:
                return limits[1]
            else:
                return None
        else:
            return None

    def getSegmentStart(self, idx):
        """return start of segment
        @param idx: segment index to query
        @type idx: int
        @rtype: Data
        """
        num = self.getNumSegments()
        if num > 0 and idx < num:
            limits = self.getSegmentLimits(idx)
            if limits is not None:
                return limits[0]
            else:
                return None
        else:
            return None

    def getStatus(self):
        """Return action completion status
        @return: action completion status stored by dispatcher if this node is a dispacted action. Low bit set is success.
        @rtype: int
        """
        return self.status

    def getTimeInserted(self):
        """Return time data was written
        @return: time data was written to this node as Uint64. Use answer.date to retrieve date/time string
        @rtype: Uint64
        """
        return self.time_inserted

    def getTags(self):
        """Return tags of this node
        @return: Tag names pointing to this node
        @rtype: ndarray
        """
        with self._lock:
            ctx = _C.c_void_p(0)
            tags = list()
            fnt = _TreeShr._TreeFindNodeTags
            fnt.restype = _C.c_void_p
            while True:
                tag_ptr = _TreeShr._TreeFindNodeTags(self.ctx,
                                                     self._nid,
                                                     _C.byref(ctx))
                if not tag_ptr:
                    break
                try:
                    value = _C.cast(tag_ptr, _C.c_char_p).value
                    tags.append(_ver.tostr(value.rstrip()))
                finally:
                    _TreeShr.TreeFree(_C.c_void_p(tag_ptr))
        tags = _arr.Array(tags)
        return tags

    def getTree(self):
        """Return Tree associated with this node
        @return: Tree associated with this node
        @rtype: Tree
        """
        return self.tree

    def getUsage(self):
        """Return usage of this node
        @return: usage of this node
        @rtype: str
        """
        return self.usage

    def hasNodeReferences(self):
        """Return True if this node contains data that includes references
        to other nodes in the same tree
        @return: True of data references other nodes
        @rtype: bool
        """
        return self.node_reference

    def hasPathReferences(self):
        """Return True if this node contains node references using paths.
        This usually means the data references nodes from other subtrees.
        @return: True if data contains node path references
        @rtype: bool
        """
        return self.path_reference

    def isChild(self):
        """Return true if this is a child node
        @return: True if this is a child node instead of a member node.
        @rtype: bool
        """
        return int(self.parent_relationship) == Nci._IS_CHILD

    def isCompressible(self):
        """Return true if node contains data which can be compressed
        @return: True of this node contains compressible data
        @rtype: bool
        """
        return self.compressible

    def isCompressOnPut(self):
        """Return true if node is set to compress on put
        @return: True if compress on put
        @rtype: bool
        """
        return self.compress_on_put

    def isCompressSegments(self):
        """Return true if node is set to compress segments"""
        return self.compress_segments

    def isDisabled(self):
        """Return true if this node is disabled (opposite of isOn)
        @return: True if node is off
        @rtype: bool
        """
        return not self.isOn()

    def isDoNotCompress(self):
        """Return true if compression is disabled for this node
        @return: True if this node has compression disabled
        @rtype: bool
        """
        return self.do_not_compress

    def isEssential(self):
        """Return true if successful action completion is essential
        @return: True if this node is marked essential.
        @rtype: bool
        """
        return self.essential

    def isIncludeInPulse(self):
        """Return true if this subtree is to be included in pulse file
        @return: True if subtree is to be included in pulse file creation.
        @rtype: bool
        """
        return self.include_in_pulse

    def isMember(self):
        """Return true if this is a member node
        @return:  True if this is a member node
        @rtype: bool
        """
        return int(self.parent_relationship) == Nci._IS_MEMBER

    def isNoWriteModel(self):
        """Return true if data storage to model is disabled for this node
        @return: Return True if storing data in this node in the model tree is disabled
        @rtype: bool
        """
        return self.no_write_model

    def isNoWriteShot(self):
        """Return true if data storage to pulse file is disabled for this node
        @return: Return True if storing data in this node in the pulse tree is disabled
        @rtype: bool
        """
        return self.no_write_shot

    def isOn(self):
        """Return True if node is turned on, False if not.
        @return: Return True if node is turned on
        @rtype: bool
        """
        return (int(self.get_flags) & 3) == 0

    def isParentDisabled(self):
        """Return True if parent is disabled
        @return: Return True if parent is turned off
        @rtype: bool
        """
        return not self.isParentOn()

    def isParentOn(self):
        """Return True if parent is on
        @return: Return True if parent is turned on
        @rtype: bool
        """
        return (int(self.get_flags) & 2) == 0

    def isSegmented(self):
        """Return true if this node contains segmented records
        @return: True if node contains segmented records
        @rtype: bool
        """
        return self.segmented

    def isSetup(self):
        """Return true if data is setup information.
        @return: True if data is setup information (originally written in the model)
        @rtype: bool
        """
        return self.setup

    def isWriteOnce(self):
        """Return true if node is set write once
        @return: Return True if data overwrite in this node is disabled
        @rtype: bool
        """
        return self.write_once

    def makeSegment(self, start, end, dim, array, idx=-1, rows_filled=-1):
        """Make a record segment
        @param start: Index of first row of data
        @type start: Data
        @param end: Index of last row of data
        @type end: Data
        @param dim: Dimension information of segment
        @type dim: Dimension
        @param array: Contents of segment
        @type array: Array
        @param idx: Target segment index, defaults to -1, i.e. append
        @type idx: int
        @param rows_filled: Rows filled, defaults to array.shape[0], i.e. full
        @type rows_filled: int
        @rtype: None
        """
        start, end, dim, array = map(_dat.Data, (start, end, dim, array))
        if rows_filled < 0:
            rows_filled = 1 if isinstance(
                array, _cmp.Compound) else array.shape[0]
        _exc.checkStatus(
                _TreeShr._TreeMakeSegment(self.ctx,
                                          self._nid,
                                          _dat.Data.byref(start),
                                          _dat.Data.byref(end),
                                          _dat.Data.byref(dim),
                                          _dat.Data.byref(array),
                                          _C.c_int32(int(idx)),
                                          _C.c_int32(int(rows_filled))))


    def makeSegmentResampled(self,start,end,dim,array,resNode,resFactor,idx=-1,rows_filled=-1):
        """Make a resampled record segment, using an average
        @param start: Index of first row of data
        @type start: Data
        @param end: Index of last row of data
        @type end: Data
        @param dim: Dimension information of segment
        @type dim: Dimension
        @param array: Contents of segment
        @type array: Array
        @param resNode: TreeNode of the node to write the resampled data into
        @type resNode: TreeNode
        @param resFactor: Number of samples to resample together
        @type resFactor: Data
        @param idx: Target segment index, defaults to -1, i.e. append
        @type idx: int
        @param rows_filled: Rows filled, defaults to array.shape[0], i.e. full
        @type rows_filled: int
        @rtype: None
        """
        start,end,dim,array = map(_dat.Data,(start,end,dim,array))
        if rows_filled<0:
            rows_filled = 1 if isinstance(array,_cmp.Compound) else array.shape[0]
        _exc.checkStatus(
                _TreeShr._TreeMakeSegmentResampled(self.ctx,
                                                   self._nid,
                                                   _dat.Data.byref(start),
                                                   _dat.Data.byref(end),
                                                   _dat.Data.byref(dim),
                                                   _dat.Data.byref(array),
                                                   _C.c_int32(int(idx)),
                                                   _C.c_int32(int(rows_filled)),
                                                   resNode._nid,
                                                   _C.c_int32(int(resFactor))))


    def makeSegmentMinMax(self,start,end,dim,array,resNode,resFactor,idx=-1,rows_filled=-1):
        """Make a resampled record segment, using the min and max values
        @param start: Index of first row of data
        @type start: Data
        @param end: Index of last row of data
        @type end: Data
        @param dim: Dimension information of segment
        @type dim: Dimension
        @param array: Contents of segment
        @type array: Array
        @param resNode: TreeNode of the node to write the resampled data into
        @type resNode: TreeNode
        @param resFactor: Number of samples to resample together
        @type resFactor: Data
        @param idx: Target segment index, defaults to -1, i.e. append
        @type idx: int
        @param rows_filled: Rows filled, defaults to array.shape[0], i.e. full
        @type rows_filled: int
        @rtype: None
        """
        start,end,dim,array = map(_dat.Data,(start,end,dim,array))
        if rows_filled<0:
            rows_filled = 1 if isinstance(array,_cmp.Compound) else array.shape[0]
        _exc.checkStatus(
                _TreeShr._TreeMakeSegmentMinMax(self.ctx,
                                                self._nid,
                                                _dat.Data.byref(start),
                                                _dat.Data.byref(end),
                                                _dat.Data.byref(dim),
                                                _dat.Data.byref(array),
                                                _C.c_int32(int(idx)),
                                                _C.c_int32(int(rows_filled)),
                                                resNode._nid,
                                                _C.c_int32(int(resFactor))))

    def move(self,parent,newname=None):
        """Move node to another location in the tree and optionally rename the node
        @param parent: New parent of this node
        @type parent: TreeNode
        @param newname: Optional new node name of this node. 1-12 characters, no path delimiters.
        @type newname: str
        @rtype: None
        """
        if newname is None:
            newname = _ver.tostr(self.node_name)
        newpath = _ver.tostr(parent.path)
        newpath += "." if self.isChild() else ":"
        newpath += newname
        _exc.checkStatus(
            _TreeShr._TreeRenameNode(self.ctx,
                                     self._nid,
                                     _ver.tobytes(newpath)))

    def putData(self, value):
        """Store data
        @param value: Data to store in this node.
        @type value: Data
        @rtype: None
        """
        if value is None:
            ref = _C.c_void_p(0)
        else:
            if isinstance(value, _dat.TreeRef) and value.__hasBadTreeReferences__(self.tree):
                data = value.__fixTreeReferences__(self.tree)
            else:
                data = _dat.Data(value)
            ref = _dat.Data.byref(data)
        _exc.checkStatus(
            _TreeShr._TreePutRecord(self.ctx,
                                    self._nid,
                                    ref,
                                    0))

    def putRow(self, bufsize, data, timestamp):
        """Load a timestamped segment row
        @param bufsize: number of rows in segment
        @type bufsize: int
        @param data: data for this row
        @type data: Array or Scalar
        @param timestamp: Timestamp of this row
        @type timestamp: Uint64
        @rtype: None
        """
        data = _dat.Data(data)
        _exc.checkStatus(
            _TreeShr._TreePutRow(self.ctx,
                                 self._nid,
                                 _C.c_int32(int(bufsize)),
                                 _C.byref(_C.c_int64(int(timestamp))),
                                 _dat.Data.byref(data)))

    def putSegment(self, data, idx=-1):
        """Load a segment in a node
        @param data: data to load into segment
        @type data: Array or Scalar
        @param idx: index into segment to load data
        @type idx: int (default: append)
        @rtype: None
        """
        data = _dat.Data(data)
        _exc.checkStatus(
            _TreeShr._TreePutSegment(self.ctx,
                                     self._nid,
                                     _C.c_int32(int(idx)),
                                     _dat.Data.byref(data)))

    def putTimestampedSegment(self, timestampArray, array):
        """Load a timestamped segment
        @param timestampArray: Array of time stamps
        @type timestampArray: Uint64Array
        @param array: Data to load into segment
        @type array: Array
        @rtype: None
        """
        timestampArray = _arr.Int64Array(timestampArray)
        array = _arr.Array(array)
        _exc.checkStatus(
            _TreeShr._TreePutTimestampedSegment(self.ctx,
                                                self._nid,
                                                timestampArray.value.ctypes,
                                                _dat.Data.byref(array)))

    def makeTimestampedSegment(self, timestampArray, array, idx=None, rows_filled=None):
        """Load a timestamped segment
        @param timestampArray: Array of time stamps
        @type timestampArray: Uint64Array
        @param array: Data to load into segment
        @type array: Array
        @param idx: Segment number
        @param idx: int
        @param rows_filled: Number of rows of segment filled with data
        @type rows_filled: int
        @rtype: None
        """
        if idx is None:
            idx = -1
        timestampArray = _arr.Int64Array(timestampArray)
        array = _arr.Array(array)
        if rows_filled is None:
            rows_filled = int(timestampArray.size)
        _exc.checkStatus(
            _TreeShr._TreeMakeTimestampedSegment(self.ctx,
                                                 self._nid,
                                                 timestampArray.value.ctypes,
                                                 _dat.Data.byref(array),
                                                 _C.c_int32(int(idx)),
                                                 _C.c_int32(int(rows_filled))))

    def removeTag(self, tag):
        """Remove a tagname from this node
        @param tag: Tagname to remove from this node
        @type tag: str
        @rtype: original type
        """
        try:
            tag = _ver.tostr(tag)
            n = self.tree.getNode('\\'+tag)
            if n.nid != self.nid:
                raise TreeNodeException(
                    "Node %s does not have a tag called %s. That tag refers to %s" % (str(self), tag, str(n)))
        except _exc.TreeNNF:
            raise TreeNodeException("Tag %s is not defined" % (tag,))
        self.tree.removeTag(tag)
        return self

    def rename(self, newname):
        """Rename node this node
        @param newname: new name of this node. 1-12 characters, no path delimiters.
        @type newname: str
        @rtype: original type
        """
        if newname.find(':') >= 0 or newname.find('.') >= 0:
            raise TreeNodeException(
                "Invalid node name, do not include path delimiters in nodename")
        with self._lock:
            olddefault = self.tree.default
            try:
                self.tree.setDefault(self.parent)
                if self.isChild():
                    newname = "."+_ver.tostr(newname)
                _exc.checkStatus(
                    _TreeShr._TreeRenameNode(self.ctx,
                                             self._nid,
                                             _C.c_char_p(_ver.tobytes(newname))))
            finally:
                self.tree.setDefault(olddefault)
        return self

    def setCompressOnPut(self, flag):
        """Set compress on put state of this node
        @param flag: State to set the compress on put characteristic
        @type flag: bool
        @rtype: original type
        """
        self.compress_on_put = flag

    def setCompressSegments(self, flag):
        """Set compress segments state of this node
        @param flag: State to set the compress segments characteristic
        @type flag: bool
        @rtype: original type
        """
        self.compress_segments = flag

    def setDefault(self):
        """Set compress segments state of this node
        @rtype: None
        """
        self.tree.setDefault(self)

    def setDoNotCompress(self, flag):
        """Set do not compress state of this node
        @param flag: True do disable compression, False to enable compression
        @type flag: bool
        @rtype: None
        """
        self.do_not_compress = flag

    def setEssential(self, flag):
        """Set essential state of this node
        @param flag: State to set the essential characteristic. This is used on action nodes when phases are dispacted.
        @type flag: bool
        @rtype: original type
        """
        self.essential = flag

    def setExtendedAttribute(self, name, value):
        """Set extended attribute of node
        @param name: Name of attribute
        @type name: str
        @param value: value of attribute
        @type value: MDSplus data type
        """
        if value is None:
            ref = _C.c_void_p(0)
        else:
            data = _dat.Data(value)
            if data.__hasBadTreeReferences__(self.tree):
                data = data.__fixTreeReferences__(self.tree)
            ref = _dat.Data.byref(data)
        _exc.checkStatus(
            _TreeShr._TreeSetXNci(self.ctx,
                                  self.nid,
                                  _C.c_char_p(_ver.tobytes(name)),
                                  ref))

    def setIncludeInPulse(self, flag):
        """Set include in pulse state of this node
        @param flag: State to set the include in pulse characteristic.
        If true and this node is the top node of a subtree the subtree will be included in the pulse.
        @type flag: bool
        @rtype: original type
        """
        self.include_in_pulse = flag

    def setNoWriteModel(self, flag):
        """Set no write model state for this node
        @param flag: State to set the no write in model characteristic.
        If true then no data can be stored in this node in the model.
        @type flag: bool
        @rtype: original type
        """
        self.no_write_model = flag

    def setNoWriteShot(self, flag):
        """Set no write shot state for this node
        @param flag: State to set the no write in shot characteristic.
        If true then no data can be stored in this node in a shot file.
        @type flag: bool
        @rtype: original type
        """
        self.no_write_shot = flag

    def setParentOn(self, flag):
        """Turn parent node on or off
        @param flag: State to set the on characteristic.
        If true then the node is turned on. If false the node is turned off.
        @type flag: bool
        @rtype: None
        """
        self.parent.on = flag

    def setOn(self, flag):
        """Turn node on or off
        @param flag: State to set the on characteristic.
        If true then the node is turned on. If false the node is turned off.
        @type flag: bool
        @rtype: None
        """
        method = _TreeShr._TreeTurnOn if flag else _TreeShr._TreeTurnOff
        _exc.checkStatus(method(self.ctx, self._nid))
        return self

    def setSegmentScale(self, scale):
        """sets the scale expression of a segmetned Node
        @param scale: expression for the data field; should contain $VALUE
        @rtype: None
        """
        if not scale is None:
            if not isinstance(scale, _dat.Data):
                scale = _dat.Data(scale)
            if isinstance(scale, _scr.Scalar):
                scale = _cmp.MULTIPLY(_cmp.dVALUE(), scale)
            elif isinstance(scale, _arr.Array):
                fac, nn = scale, scale.size
                scale = None
                for n in range(nn-1, -1, -1):
                    if n > 1:
                        next = _cmp.MULTIPLY(
                            _cmp.POWER(_cmp.dVALUE(), n), fac[n])
                    if n == 1:
                        next = _cmp.MULTIPLY(_cmp.dVALUE(), fac[n])
                    else:
                        next = fac[n]
                    if scale is None:
                        scale = next
                    else:
                        scale = _cmp.ADD(scale, next)
        _exc.checkStatus(
            _TreeShr._TreeSetSegmentScale(self.ctx,
                                          self._nid,
                                          _dat.Data.byref(scale)))

    def setSubtree(self, flag):
        """Enable/Disable node as a subtree
        @param flag: True to make node a subtree reference.
        Node must be a child node with no descendants.
        @type flag: bool
        @rtype: original type
        """
        method = _TreeShr._TreeSetSubtree if flag else _TreeShr._TreeSetNoSubtree
        _exc.checkStatus(method(self.ctx, self._nid))
        return self

    def setUsage(self, usage):
        """Set the usage of a node
        @param usage: Usage string.
        @type usage: str
        @rtype: original type
        """
        try:
            usagenum = _usage_table[usage.upper()]
        except KeyError:
            raise UsageError(usage)
        _exc.checkStatus(
            _TreeShr._TreeSetUsage(self.ctx,
                                   self._nid,
                                   _C.c_int32(usagenum)))
        return self

    def setCompressionMethod(self, compression_method):
        """Set the usage of a node
        @param compression_method: name of compression method from predfined list.
        @type usage: str
        @rtype: original type
        """
        try:
          compressionmethodnum = _compression_methods_table[compression_method.lower()]
        except KeyError:
          raise CompressionMethodError(compression_method)
        self._setNci(Nci.COMPRESSION_METHOD[0], compressionmethodnum)
     
    def setTree(self, tree):
        """Set Tree associated with this node
        @param tree: Tree instance to associated with this node
        @type tree: Tree
        @rtype: original type
        """
        self.tree = tree
        return self

    def setWriteOnce(self, flag):
        """Set write once state of node
        @param flag: State to set the write once characteristic. If true then data can only be written if the node is empty.
        @type flag: bool
        @rtype: original type
        """
        self.write_once = flag

    def setRowsFilled(self, rows_filled=-1):
        """ updates the rows_filled value; influences the length of the last segment
        @param rows_filled: length of last segment
        @type rows_filled: int
        @rtype: None
        """
        _exc.checkStatus(
            _TreeShr._TreeSetRowsFilled(self.ctx,
                                        self._nid,
                                        _C.c_int32(int(rows_filled))))

    def tcl(self, cmd):
        """Issue a tcl command with this node being the default node
        @param cmd: tcl command string
        @type cmd: str
        @rtype: int
        """
        olddef = self.tree.default
        self.tree.default = self
        try:
            return self.tree.tcl(cmd)
        finally:
            self.tree.default = olddef

    def updateSegment(self, start, end, dim, idx):
        """Update a segment
        @param start: index of first row of segment
        @type start: Data
        @param end: index of last row of segment
        @type end: Data
        @param dim: Dimension of segment
        @type dim: Dimension
        @param idx: Index of segment
        @type idx: int
        @rtype: None
        """
        start, end, dim = map(_dat.Data, (start, end, dim))
        _exc.checkStatus(
            _TreeShr._TreeUpdateSegment(self.ctx,
                                        self._nid,
                                        _dat.Data.byref(start),
                                        _dat.Data.byref(end),
                                        _dat.Data.byref(dim),
                                        _C.c_int32(int(idx))))

    def updateSegmentLimits(self, start, end, idx):
        """Update a segment
        @param start: index of first row of segment
        @type start: Data
        @param end: index of last row of segment
        @type end: Data
        @param idx: Index of segment
        @type idx: int
        @rtype: None
        """
        start, end = map(_dat.Data, (start, end))
        _exc.checkStatus(
            _TreeShr._TreeUpdateSegment(self.ctx,
                                        self._nid,
                                        _dat.Data.byref(start),
                                        _dat.Data.byref(end),
                                        None,
                                        _C.c_int32(int(idx))))


class TreePath(TreeNode):  # HINT: TreePath begin
    """Class to represent an MDSplus node reference (path)."""
    dtype_id = 193

    def __init__(self, path, *tree, **kw):
        if self is path:
            return
        self.tree_path = str(path)
        if len(tree) < 1 or not isinstance(tree[0], (Tree,)):
            self.tree = Tree(*tree)
        else:
            self.tree = tree[0]
        try:
            self._nid = self.tree.getNode(self.tree_path)._nid
        except:
            self._nid = None
        return

    def copy(self, mode='NORMAL'):
        """returns the node with a local private instance of the tree opend in specified mode
        @param mode: Optional mode, one of 'Normal','Edit','New','Readonly'
        @type mode: str
        @rtype: TreeNode
        """
        return self.__class__(self.tree_path, self.tree.copy(mode))

    def __hasBadTreeReferences__(self, tree):
        return False

    def __str__(self):
        """Convert path to string."""
        return self.tree_path

    @property
    def _descriptor(self):
        d = _dsc.DescriptorS()
        d.length = len(self.tree_path)
        d.dtype = self.dtype_id
        d.pointer = _C.cast(_C.c_char_p(
            _ver.tobytes(self.tree_path)), _C.c_void_p)
        d.tree = self.tree
        return _cmp.Compound._descriptorWithProps(self, d)

    @classmethod
    def fromDescriptor(cls, d):
        return cls(_ver.tostr(_C.cast(d.pointer, _C.POINTER(_C.c_char*d.length)).contents.value), d.tree)


class TreeNodeArray(_dat.TreeRef, _arr.Int32Array):  # HINT: TreeNodeArray begin
    def __new__(cls, nids, *tree, **kw):
        return super(TreeNodeArray, cls).__new__(cls, nids)

    def __init__(self, nids, *tree, **kw):
        if self is nids:
            return
        if isinstance(nids, _C.Array):
            try:
                nids = _N.ctypeslib.as_array(nids)
            except Exception:
                pass
        nids = _N.array(nids)
        if len(nids.shape) == 0:  # happens if value has been a scalar, e.g. int
            nids = nids.reshape(1)
        self._value = _N.array(nids, dtype=_N.int32, copy=False)
        if 'tree' in kw:
            self.tree = tree
        elif isinstance(tree[0], (Tree,)):
            self.tree = tree[0]
        else:
            self.tree = Tree(*tree)

    def copy(self, mode='NORMAL'):
        """returns the node array with a local private instance of the tree opend in specified mode
        @param mode: Optional mode, one of 'Normal','Edit','New','Readonly'
        @type mode: str
        @rtype: TreeNodeArray
        """
        return self.__class__(self._value, self.tree.copy(mode))

    def __eq__(self, obj):
        if isinstance(obj, (TreeNodeArray,)):
            if self.tree == obj.tree:
                return self._value == obj._value
            _N.zeros((max(self._value.size, obj._value.size),), _N.bool)
        return _N.zeros_like(self._value, _N.bool)

    def __getitem__(self, n):
        """Return TreeNode from mdsarray. array[n]
        @param n: Index into array beginning with index 0
        @type n: int
        @return: node
        @rtype: TreeNode
        """
        return TreeNode(self._value[n], self.tree)

    def __add__(self, x):
        if isinstance(x, (TreeNodeArray,)) and self.tree == x.tree:
            return TreeNodeArray(_N.concatenate((self._value, x._value)), self.tree)
        raise Exception('You can only add TreeNodeArrays of the same tree')

    @property
    def nid_number(self):
        return _arr.Array(self._value)

    def getPath(self):
        """Return tuple of node names"
        @return: Node names
        @rtype: tuple of String's
        """
        return self.path

    def getFullPath(self):
        """Return tuple of fullpaths
        @return: Full node paths
        @rtype: tuple of String's
        """
        return self.fullpath

    def setOn(self, flag):
        """Turn nodes on or off
        @param flag: True to turn nodes on, False to turn them off
        @type flag: Bool
        @rtype: None
        """
        for node in self:
            node.on = flag

    def getLength(self):
        """Return data lengths
        @return: tuple of data lengths
        @rtype: tuple of Int32's
        """
        return self.length

    def getCompressedLength(self):
        """Return compressed data lengths
        @return: tuple of compressed data lengths
        @rtype: tuple of Int32's
        """
        return self.rlength

    def setWriteOnce(self, flag):
        """Set nodes write once
        @rtype: original type
        """
        for node in self:
            node.write_once = flag
        return self

    def isCompressOnPut(self):
        """Is nodes set to compress on put
        @return: state of compress on put flags
        @rtype: tuple of bools
        """
        return self.compress_on_put

    def setCompressOnPut(self, flag):
        """Set compress on put flag
        @param flag: True if compress on put, False if not
        @type flag: bool
        """
        for node in self:
            node.compress_on_put = flag
        return self

    def setNoWriteModel(self, flag):
        """Set no write model flag
        @param flag: True to disallow writing to model
        @type flag: bool
        @rtype: original type
        """
        for node in self:
            node.no_write_model = flag
        return self

    def setNoWriteShot(self, flag):
        """set no write shot flags
        @param flag: True if setting no write shot
        @type flag: bool
        @rtype: original type
        """
        for node in self:
            node.no_write_shot = flag
        return self

    def getUsage(self):
        """Get usage of nodes
        @return: Usage
        @rtype: StringArray
        """
        return self.usage

    def __getattr__(self, name):
        try:
            return self.tree.tdiExecute('getnci($,$)', self._value, name)
        except:
            ans = []
            for node in self:
                val = node.__getattribute__(name)
                if isinstance(val, _dat.Data):
                    val = val.data()
                ans.append(val)
            try:
                return _arr.Array(ans)
            except:
                return _apd.List(*ans)


class cached_property(object):
    """ converts to property with cache
    cache_on_set: controls if setter will set or clear cache
    class MYDEVICE(Device):
        @cached_property
        def _trigger(self): return self.trigger
    del(self._trigger)  <=> clear cache
    """

    def __init__(self, target=None, is_property=False, **opt):
        mode = opt.get('mode', 0)
        self.lock = _threading.Lock()
        self.cache_on_set = (mode & 1) > 0
        if target:
            if is_property:
                self.target = target
            else:
                self.target = property(target)

    def __call__(self, method):
        if hasattr(self, 'target'):
            raise _exc.MdsException("cached_property.target already set")
        self.target = property(method)
        return self

    def __get__(self, inst, cls):
        if inst is None:
            return self
        with self.lock:
            if self.target in inst.__dict__:
                return inst.__dict__[self.target]
            else:
                data = self.target.__get__(inst, cls)
                inst.__dict__[self.target] = data
                return data

    def __set__(self, inst, value):
        self.target.__set__(inst, value)
        with self.lock:
            if (self.cache_on_set):
                inst.__dict__[self.target] = value
            elif self.target in inst.__dict__:
                del(inst.__dict__[self.target])

    def __delete__(self, inst):
        with self.lock:
            if self.target in inst.__dict__:
                del(inst.__dict__[self.target])


class mdsrecord(object):
    """ turns method into property
    expects method that return MDSplus node
    class MYDEVICE(Device):
        @mdsrecord(filter=float, default=0)
        def _trigger(self): return self.trigger
    ...
    dev = tree.MYDEVICE
    dev._trigger = 5   <=>   dev.trigger.record = 5
    a = dev._trigger   <=>   a = float(dev.trigger.record)
    del(dev._trigger)  <=>   dev.trigger.record = None
    # if trigger contains no data _trigger defaults to 0
    """
    @classmethod
    def smart_filter(cls, filt):
        """Generate filter from str, bytes, [str], [float], and others."""
        if not filt:
            return _dat.Data.data
        if filt == str:
            return cls.str
        if filt == bytes:
            return cls.bytes
        if isinstance(filt, list):
            if not filt:
                def filter(a):
                    return a.data().flatten().tolist()
                return filter()
            if filt[0] == int:
                return cls.int_list
            if filt[0] == float:
                return cls.float_list
            if filt[0] == str:
                return cls.str_list
            if filt[0] == bytes:
                return cls.bytes_list
            efilt = cls.smart_filter(filt[0])

            def filter(a):
                return [efilt(e) for e in a]
            return filter
        return filt

    def __new__(cls, *target, **opt):
        """Use new so it can return cached_property."""
        self = object.__new__(cls)
        self.filter = opt.get("filter", None)
        if self.filter:
            self.filter = cls.smart_filter(self.filter)
        try:
            self.default = [opt['default']]
        except KeyError:
            self.default = []
        self.cached = opt.get("cached", None)
        if target:
            return self(target[0])
        return self

    def __call__(self, target):
        if hasattr(self, 'target'):
            raise _exc.MdsException("mdsrecord.target already set")
        self.target = target
        if self.cached is None:
            return self
        return cached_property(self, True, mode=self.cached)

    def __get__(self, inst, cls):
        if inst is None:
            return self
        node = self.target.__get__(inst, cls)()
        try:
            data = node.getRecord()
        except _exc.TreeNODATA:
            if self.default:
                return self.default[0]
            raise
        if self.filter is None:
            return data
        return self.filter(data)

    def __set__(self, inst, value):
        node = self.target.__get__(inst)()
        node.putData(value)

    def __delete__(self, inst):
        node = self.target.__get__(inst)()
        node.deleteData()

    # useful filters: flattened lists and python2/3 stable chars
    @staticmethod
    def int_list(node): return _cmp.INT(node).data().flatten().tolist()

    @staticmethod
    def float_list(node): return _cmp.FLOAT(node).data().flatten().tolist()

    @staticmethod
    def bytes(node): return _ver.tobytes(node.data())

    @staticmethod
    def bytes_list(node):
        return [_ver.tobytes(x).rstrip()
                for x in node.data().flatten()]

    @staticmethod
    def str(node): return _ver.tostr(node.data())

    @staticmethod
    def str_list(node):
        return [_ver.tostr(x).rstrip()
                for x in node.data().flatten()]


def with_mdsrecords(cls):
    """ decorator for device class
    adds mdsrecord for each part in parts field
    """
    def genfun(field, part):
        @mdsrecord(**part)
        def _(self): return self.__getattr__(field)
        return _
    for part in cls.parts:
        field = part['path'].lower().replace('.', '_').replace(':', '_')
        fun = genfun(field[1:], part)
        setattr(cls, field, fun)
    return cls


class Device(TreeNode):  # HINT: Device begin
    """Used for device support classes. Provides ORIGINAL_PART_NAME, PART_NAME and Add methods and allows referencing of subnodes as conglomerate node attributes.

    Use this class as a superclass for device support classes. When creating a device support class include a class attribute called "parts"
    which describe the subnodes of your device implementation. The parts attribute should be a list or tuple of dict objects where each dict is a
    description of each subnode. The dict object should include a minimum of a 'path' key whose value is the relative path of the node (be sure to
    include the leading period or colon) and a 'type' key whose value is the usage type of the node. In addition you may optionally specify a
    'value' key whose value is the actual value to store into the node when it is first added in the tree. Instead of a 'value' key, you can
    provide a 'valueExpr' key whose value is a string which is python code to be evaluated before writing the result into the node. Use a valueExpr
    when you need to include references to other nodes in the device. Lastly the dict instance may contain an 'options' key whose values are
    node options specified as a tuple of strings. Note if you only specify one option include a trailing comma in the tuple.The "parts" attribute
    is used to implement the Add and PART_NAME and ORIGNAL_PART_NAME methods of the subclass.

    You can also include a part_dict class attribute consisting of a dict() instance whose keys are attribute names and whose values are nid
    offsets. If you do not provide a part_dict attribute then one will be created from the part_names attribute where the part names are converted
    to lowercase and the colons and periods are replaced with underscores. Referencing a part name will return another instance of the same
    device with that node as the node in the Device subclass instance. The Device class also supports the part_name and original_part_name
    attributes which is the same as doing devinstance.PART_NAME(None). NOTE: Device subclass names MUST BE UPPERCASE!

    Sample usage1::

       from MDSplus import Device

       class MYDEV(Device):
           parts=[{'path':':COMMENT','type':'text'},
                  {'path':':INIT_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch(2,'CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",
                  'options':('no_write_shot',)},
                  {'path':':STORE_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch(2,'CAMAC_SERVER','STORE',50,None),Method(None,'STORE',head))",
                  'options':('no_write_shot',)},
                  {'path':'.SETTINGS','type':'structure'},
                  {'path':'.SETTINGS:KNOB1','type':'numeric'},
                  {'path':'.SIGNALS','type':'structure'},
                  {'path':'.SIGNALS:CHANNEL_1','type':'signal','options':('no_write_model','write_once')}]

           def init(self,arg):
               knob1=self.settings_knob1.record
               return 1

           def store(self,arg):
               from MDSplus import Signal
               self.signals_channel_1=Signal(32,None,42)

    Sample usage2::

       from MDSplus import Device

           parts=[{'path':':COMMENT','type':'text'},
                  {'path':':INIT_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch(2,'CAMAC_SERVER','INIT',50,None),Method(None,'INIT',head))",
                  'options':('no_write_shot',)},
                  {'path':':STORE_ACTION','type':'action',
                  'valueExpr':"Action(Dispatch(2,'CAMAC_SERVER','STORE',50,None),Method(None,'STORE',head))",
                  'options':('no_write_shot',)},
                  {'path':'.SETTINGS','type':'structure'},
                  {'path':'.SETTINGS:KNOB1','type':'numeric'},
                  {'path':'.SIGNALS','type':'structure'},
                  {'path':'.SIGNALS:CHANNEL_1','type':'signal','options':('no_write_model','write_once')}]

           part_dict={'knob1':5,'chan1':7}

           def init(self,arg):
               knob1=self.knob1.record
               return 1

           def store(self,arg):
               from MDSplus import Signal
               self.chan1=Signal(32,None,42)

    If you need to reference attributes using computed names you can do something like::

        for i in range(16):
            self.__setattr__('signals_channel_%02d' % (i+1,),Signal(...))
    """
    try:
        debug = int(_mds.getenv('DEBUG_DEVICES', '0'))
    except ValueError:
        _sys.stdout.write("MDSplus.Device Error: Invalid DEBUG_DEVICES '%s'" %
                          _mds.getenv('DEBUG_DEVICES'))
        debug = 0
    gtkThread = None

    def dprint(self, debuglevel, fmt, *args):
        if debuglevel > self.debug:
            return
        now = _time.time()
        line = str(fmt) if not args else (fmt % args)
        _sys.stdout.write('%.3f: [%s] %s\n' % (now, self, line))
        _sys.stdout.flush()

    """ debug safe import """
    if debug == 0:
        @staticmethod
        def _debugDevice(device):
            return device
    elif debug < 0:
        @staticmethod
        def _debugDevice(dev):
            if not (isinstance(dev, (type,)) and issubclass(dev, (Device,))):
                return dev
            from types import FunctionType

            def dummy(self, *args, **kvargs): """dummy"""
            db = {}
            for d in dev.mro()[-5::-1]:  # mro[-4] is Device
                for k, v in d.__dict__.items():
                    if isinstance(v, (FunctionType,)):
                        db[k] = dummy
                    else:
                        db[k] = v
            return type(dev.__name__, (Device,), db)
    else:
        @staticmethod
        def _debugDevice(device): return device

    @staticmethod
    def _mimport(loc, glob, filename, name=None):
        debug = Device.debug & 8
        if isinstance(name, (tuple, list)):
            for n in name:
                Device._mimport(loc, glob, filename, n)
            return
        if name is None:
            name = filename
        if debug:
            _sys.stdout.write('loading %-21s' % (name+':',))
        try:
            try:
                device = __import__(filename, glob, fromlist=[
                                    name], level=1).__getattribute__(name)
            except Exception:
                device = __import__(filename, glob, fromlist=[
                                    name]).__getattribute__(name)
            if debug:
                _sys.stdout.write(' successful\n')
            loc[name] = Device._debugDevice(device)
        except Exception as exc:
            if debug:
                _sys.stdout.write(' failed: %s\n' % (exc,))
        finally:
            if debug:
                _sys.stdout.flush()
    """ /debug safe import """
    parts = []
    part_names = tuple()
    part_dict = {}
    __initialized = set()

    def __new__(cls, node, tree=None, head=0, *a, **kw):
        """Create a Device class instance.

        Initialize part_dict class attribute if necessary.
        @param node: Node of device
        @type node: TreeNode
        @return: Instance of the device subclass
        @rtype: Device subclass instance
        """

        if cls is Device:
            try:
                head = TreeNode(
                    node.conglomerate_nids.nid_number[0], node.tree, 0)
                return head.getDevice(head)
            except Exception:
                raise TypeError("Cannot create instances of Device class")
        elif not cls in cls.__initialized:
            cls.part_names = tuple(elt['path'] for elt in cls.parts)
            cls.part_dict = {}  # we need to reinit dict to get a private one
            for i, partname in enumerate(cls.part_names):
                try:
                    cls.part_dict[partname[1:].lower().replace(
                        ':', '_').replace('.', '_')] = i+1
                except Exception as ex:
                    print(ex)
            cls.__initialized.add(cls)
        return super(Device, cls).__new__(cls, node, tree, head)

    def __init__(self, node, tree=None, head=0, *a, **kw):
        """Initialize a Device instance.

        @param node: Conglomerate node of this device
        @type node: TreeNode
        @rtype: None
        """
        if self is node:
            return
        if isinstance(node, TreeNode):
            super(Device, self).__init__(node)
        else:
            super(Device, self).__init__(node, tree, head)

    def copy(self, mode='NORMAL'):
        """Make a thread safe copy of this Device node."""
        head = 0 if self.head == 0 else None
        new = self.__class__(TreeNode(self.nid, self.tree.copy(mode), head))
        for k, v in self.__dict__.items():
            if not k in new.__dict__:
                new.__dict__[k] = self.__dict__[k]
        return new

    @property
    def fullhelp_str(self):
        try:
            if self is self.head:
                return help(self.__class__)
            part = self.parts[int(self.conglomerate_elt)-2]
            msg = ['%s\n' % ('-'*64)]
            msg.append('original part:     %-10s <HEAD>%s' %
                       (part['type'].upper(), part['path'].upper()))
            flags = Flags(self.get_flags)
            if flags.no_write_model and flags.no_write_shot:
                msg.append('node is read only.')
            elif not flags.write_once:
                if flags.no_write_shot:  # not flags.no_write_model
                    msg.append('node content can be edited in the model tree.')
                elif flags.no_write_model:  # not flags.no_write_shot
                    msg.append('node content can be edited in the shot tree.')
                else:
                    msg.append('node content is editable.')
            else:
                if flags.no_write_shot:  # not flags.no_write_model
                    msg.append('node content is a static setting.')
                elif flags.no_write_model:  # not flags.no_write_shot
                    msg.append(
                        'node content is a measurement/proces parameter.')
            if flags.setup_information:
                msg.append('node contains setup information.')
            if 'value' in part:
                msg.append('default value:     %s' % repr(part['value']))
            if 'valueExpr' in part:
                msg.append('default valueExpr: %s' % part['valueExpr'])
            if 'help' in part:
                msg.append('device help:\n\n%s' % (part['help']))
            msg.append('\n%s' % ('-'*64))
            return '\n'.join(msg)
        except AttributeError as e:
            raise _exc.MdsException(e.message)

    def fullhelp(self): print(self.fullhelp_str)

    @property
    def help(self):
        if self is self.head:
            return 'head of %s' % self.__class__.__name__
        part = self.parts[int(self.conglomerate_elt)-2]
        return part.get('help', '%-10s <HEAD>%s' % (part['type'].upper(), part['path'].upper()))

    @property
    def deref(self): return self

    @property
    def _unique(self): return "%s:%d:%d" % (
        self.tree.tree, self.tree.shot, self.head.nid)
    _persistent = dict()

    @property
    def persistent(self): return self._persistent.get(self._unique, None)

    @persistent.setter
    def persistent(self, value):
        if value is None:
            del(self._persistent[self._unique])
        else:
            self._persistent[self._unique] = value

    def __getattr__(self, name):
        """Return TreeNode of subpart if name matches mangled node name.
        @param name: part name. Node path with colons and periods replaced by underscores.
        @type name: str
        @return: Device instance of device part
        @rtype: Device
        """
        if name in self.part_dict:
            head = self if self._head == 0 else self.head
            return self.__class__(TreeNode(self.part_dict[name]+self.head.nid, self.tree, head))
        return super(Device, self).__getattr__(name)

    def __setattr__(self, name, value):
        """Set value into device subnode if name matches a mangled subpart node name.
        Otherwise assign value to class instance attribute.
        @param name: Name of attribute or device subpart
        @type name: str
        @param value: Value of the attribute or device subpart
        @type value: varied
        @rtype: None
        """
        def is_in_dicts(name, cls):
            for c in cls.mro()[:-1]:
                if name in c.__dict__:
                    return True
            return False
        from inspect import stack
        if name in self.part_dict:
            head = self if self._head == 0 else self.head
            TreeNode(self.part_dict[name]+self.head.nid,
                     self.tree, head).record = value
        elif (name.startswith('_')
              or name in self.__dict__
              or is_in_dicts(name, self.__class__)
              or isinstance(stack()[1][0].f_locals.get('self', None), Device)):
            super(Device, self).__setattr__(name, value)
        else:
            print("""WARNING: your tried to add the attribute or write to the subnode '%s' of '%s'.
This is a deprecated action for Device nodes outside of Device methods. You should prefix the attribute with '_'.
If you did intend to write to a subnode of the device you should check the proper path of the node: TreeNNF.
""" % (name, self.path))

    @classmethod
    def getImportString(cls):
        basemodule = cls.__module__.split('.', 2)[0]
        try:
            import_string = "from %s import %s" % (basemodule, cls.__name__)
            # test if this would import same class
            env = {}
            exec(compile(import_string, '<string>', 'exec'), {}, env)
            if env.get(cls.__name__, None) == cls:
                return import_string
        except ImportError:
            pass
        try:
            Device.PyDevice(basemodule, cls.__name__)
            if basemodule.lower() != cls.__name__.lower():
                return "Device.PyDevice('%s','%s')" % (basemodule, cls.__name__)
            else:
                return None
        except _exc.DevPYDEVICE_NOT_FOUND:
            return "from %s import %s" % (cls.__module__, cls.__name__)

    @classmethod
    def __read_source(cls, model, sourcefile=None):
        """ sourcefile must be a string, a Device class, or None"""
        if not isinstance(sourcefile, _ver.basestring):
            if sourcefile is None:
                sourcefile = cls.PyDevice(model)
            import inspect
            sourcefile = inspect.getsourcefile(sourcefile)
        source = _N.fromfile(sourcefile, _N.uint8)
        if _ver.iswin:  # convert to linux line-ending
            _N.delete(source, _N.argwhere(source == 13))  # ord('\r') == 13
        return source

    def check_source(self, sourcefile=None):
        source = self.head.record.qualifiers.data()
        if isinstance(source, _N.ndarray) and source.dtype == _N.uint8:
            import difflib
            inrecord = _ver.tostr(source.tostring()).splitlines(1)
            infile = _ver.tostr(self.__read_source(
                self.__class__.__name__, sourcefile).tostring()).splitlines(1)
            diff = difflib.unified_diff(inrecord, infile)
            diff = ''.join(diff)
            return diff
        return "No source stored in record."

    @classmethod
    def __add_source(cls, head, model, sourcefile=None):
        """ sourcefile must be a string, a Device class, or None"""
        source = cls.__read_source(model, sourcefile)
        hashvalue = _scr.Uint64(_ver.hash64(source))
        source = _arr.Uint8Array(source)
        head.record = _cmp.Conglom('__python__', model, hashvalue, source)

    def _update_source(self, sourcefile=None):
        head = self.head
        head.write_once = False
        try:
            self.__add_source(head, head.__class__.__name__, sourcefile)
        finally:
            head.write_once = True

    @classmethod
    def Add(cls, tree, name, add_source=False):
        """Used to add a device instance to an MDSplus tree.
        This method is invoked when a device is added to the tree when using utilities like mdstcl and the traverser.
        For this to work the device class name (uppercase only) and the package name must be returned in the MdsDevices tdi function.
        Also the Device subclass must include the parts attribute which is a list or tuple containing one dict instance per subnode of the device.
        The dict instance should include a 'path' key set to the relative node name path of the subnode. a 'type' key set to the usage string of
        the subnode and optionally a 'value' key or a 'valueExpr' key containing a value to initialized the node or a string containing python
        code which when evaluated during the adding of the device after the subnode has been created produces a data item to store in the node.
        And finally the dict instance can contain an 'options' key which should contain a list or tuple of strings of node attributes which will be turned
        on (i.e. write_once).
        """
        if cls is Device:
            raise _exc.MdsException("Cannot add super class Device.")
        parent = tree
        if isinstance(tree, TreeNode):
            tree = tree.tree
        _TreeShr._TreeStartConglomerate(tree.ctx, len(cls.parts)+1)
        if isinstance(name, _scr.Ident):
            name = name.data()
        head = parent.addNode(name, 'DEVICE')
        if add_source is None:
            add_source = _mds.getenv("PYDEVICE_ADD_SOURCE", "no").lower()
            add_source = not (add_source == "no" or add_source ==
                              "0" or add_source == "off")
        if add_source:
            try:
                cls.__add_source(head, cls.__name__, cls)
            except Exception as e:
                _sys.stderr.write(
                    "Could not find source code for %s: %s\n" % (cls.__name__, str(e)))
                head.record = _cmp.Conglom(
                    '__python__', cls.__name__, None, cls.getImportString())
        else:
            head.record = _cmp.Conglom(
                '__python__', cls.__name__, None, cls.getImportString())
        head = TreeNode(head)
        head.write_once = True
        glob = _mimport('__init__').load_package({})
        glob['tree'] = tree
        glob['path'] = head.path
        glob['head'] = head
        for elt in cls.parts:  # first add all nodes
            node = head.addNode(elt['path'], elt.get('type', 'none'))
        for elt in cls.parts:  # then you can reference them in valueExpr
            try:
                node = head.getNode(elt['path'])
                if 'value' in elt:
                    if Device.debug:
                        print(node, node.usage, elt['value'])
                    node.record = elt['value']
                elif 'valueExpr' in elt:
                    glob['node'] = node
                    if Device.debug:
                        print(node, node.usage, elt['valueExpr'])
                    node.record = eval(elt['valueExpr'], glob)
                if 'options' in elt:
                    for option in elt['options']:
                        node.__setattr__(option, True)
                if 'ext_options' in elt:
                   if isinstance(elt['ext_options'], dict):
                       for ext_option in elt['ext_options'].keys():
                           node.setExtendedAttribute(ext_option, elt['ext_options'][ext_option])
            except:
                _sys.stderr.write('ERROR: %s\n' % str(elt))
                raise
        _TreeShr._TreeEndConglomerate(tree.ctx)
        return head

    def dw_setup(self, *args):
        """Bring up a glade setup interface if one exists in the same package as the one providing the device subclass

        The gtk.main() procedure must be run in a separate thread to avoid locking the main program.
        """
        try:
            _widgets = _mimport('widgets')
            import gtk
            import gtk.glade
            import os
            import inspect
            import threading
            import sys

            class gtk_main(threading.Thread):
                def run(self):
                    gtk.main()

            class MyOut:
                def __init__(self):
                    self.content = []

                def write(self, string):
                    self.content.append(string)

            gtk.gdk.threads_init()
            out = MyOut()
            sys.stdout = out
            sys.stderr = out
            window = gtk.glade.XML(os.path.dirname(inspect.getsourcefile(
                self.__class__))+os.sep+self.__class__.__name__+'.glade').get_widget(self.__class__.__name__.lower())
            sys.stdout = sys.__stdout__
            sys.stderr = sys.__stderr__
            window.device_node = self
            window.set_title(window.get_title()+' - ' +
                             str(self)+' - '+str(self.tree))
            _widgets.MDSplusWidget.doToAll(window, "reset")
        except Exception as exc:
            raise _exc.MdsException("No setup available, %s" % (str(exc),))

        window.connect("destroy", self.onSetupWindowClose)
        window.show_all()
        if Device.gtkThread is None or not Device.gtkThread.isAlive():
            Device.gtkThread = gtk_main()
            Device.gtkThread.start()
        return _exc.MDSplusSUCCESS.status
    DW_SETUP = dw_setup

    def onSetupWindowClose(self, window):
        import gtk
        windows = [toplevel for toplevel in gtk.window_list_toplevels()
                   if toplevel.get_property('type') == gtk.WINDOW_TOPLEVEL]
        if len(windows) == 1:
            gtk.main_quit()

    @classmethod
    def waitForSetups(cls):
        cls.gtkThread.join()

    @staticmethod
    def __cached():
        import sys
        import os
        path = _mds.getenv("MDS_PYDEVICE_PATH", None)
        if not path:
            path = os.path.join(
                _mds.getenv("MDSPLUS_DIR", "/usr/local/mdsplus"), "pydevices")
        if not path == Device.__cached_mds_pydevice_path:
            Device.__cached_mds_pydevice_path = path
            Device.__cached_py_devices = None
        return sys, os, path

    __cached_mds_pydevice_path = ""
    __cached_py_devices = None
    __cached_lock = _threading.Lock()

    @classmethod
    def importPyDeviceClass(cls, name):
        """Find a device support module with a case insensitive lookup of
        'model'.py in the MDS_PYDEVICE_PATH environment variable search list.
        @rtype: Device subclass
        """
        name = name.upper()
        py_devices = cls.findPyDevices()
        if name in py_devices:
            return py_devices[name]
        raise _exc.DevPYDEVICE_NOT_FOUND

    @classmethod
    def findPyDevices(cls):
        """Find all device support models in the MDS_PYDEVICE_PATH environment variable search list.
        @rtype: dict of device_name:device_class
        """
        def process_file(fname, ans):
            if fname.startswith("_"):
                return  # filter: hidden
            if not fname.endswith('.py'):
                return  # filter: other file types
            try:
                modname = fname[:-3]
                module = __import__(modname)
                for k, v in module.__dict__.items():
                    try:
                        if k.startswith("_"):
                            continue  # filter: no private classes
                        if not isinstance(v, (Device.__class__,)):
                            continue  # filter: only classes
                        if not issubclass(v, Device):
                            continue  # filter: only device classes
                        if not v.__module__ == modname:
                            continue  # filter: no super classes
                        kupper = k.upper()
                        if not kupper in ans:
                            ans[kupper] = v
                        else:
                            print("Device '%s' in '%s' already imported from '%s'." % (
                                k, modname, ans[k].__module__))
                    except Exception as e:
                        print('Error importing %s from %s: %s' % (k, fname, e))
            except Exception as e:
                print('Error importing from %s: %s' % (fname, e))

        with cls.__cached_lock:
            sys, os, path = Device.__cached()
            if Device.__cached_py_devices is None:
                ans = {}
                if path:
                    sys.path.insert(0, '')
                    try:
                        for part in path.split(';'):
                            for dp, dn, fn in os.walk(part):
                                if os.path.basename(dp).startswith('_'):
                                    continue
                                sys.path[0] = dp
                                for fname in fn:
                                    process_file(fname, ans)
                    finally:
                        del(sys.path[0])
                Device.__cached_py_devices = ans
                sys.stdout.flush()
            return Device.__cached_py_devices

    @classmethod
    def PyDevice(cls, module, model=None):
        """Find a python device class by:
          1. finding the model in the list defined by
           the tdi function, MdsDevices.
          2. try importing the package for the model and calling its Add method.
        The StringArray returned by MdsDevices() contains String instances
        containing blank filled values containing an \0 character embedded.
        These Strings have to be manipulated to produce simple str() values.
        @rtype: Device subclass
        """
        cls_list = []
        if model is None:
            model = module
            MODEL = model.upper()
            models = _dat.Data.execute('MdsDevices()').data()
            for idx in range(len(models)):
                modname = _ver.tostr(models[idx][0].tolist().rstrip())
                MODNAME = modname.upper()
                if MODEL == MODNAME:
                    package = _ver.tostr(models[idx][1].tolist().rstrip())
                    if package == "pydevice":
                        break
                    try:
                        return __import__(package).__dict__[modname]
                    except ImportError:
                        pass
                    except KeyError:
                        pass
            return cls.importPyDeviceClass(model)
        MODEL = model.upper()
        module = __import__(module)
        if module is None:
            raise _exc.DevPYDEVICE_NOT_FOUND
        cls_list = [v for k, v in module.__dict__.items()
                    if isinstance(v, (Device.__class__,))
                    and issubclass(v, Device)
                    and k.upper() == MODEL]
        if len(cls_list) == 1:
            return cls_list[0]
        if len(cls_list) > 1:
            print ("Error pydevice %s: Name ambiguous (%s)" %
                   (model, ','.join(cls_list)))
            return cls_list[0]
        raise _exc.DevPYDEVICE_NOT_FOUND


############# dtype to classes ##################################
_dsc.dtypeToClass[TreeNode.dtype_id] = TreeNode
_dsc.dtypeToClass[TreePath.dtype_id] = TreePath
