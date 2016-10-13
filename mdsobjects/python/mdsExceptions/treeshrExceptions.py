
########################################################
# This module was generated using mdsshr/gen_device.py
# To add new status messages modify:
#     treeshr_messages.xml
# and then in mdsshr do:
#     python gen_devices.py
########################################################

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

MDSplusException=_mimport('__init__').MDSplusException


class TreeException(MDSplusException):
  fac="Tree"


class TreeALREADY_OFF(TreeException):
  status=265388075
  message="Node is already OFF"
  msgnam="ALREADY_OFF"



class TreeALREADY_ON(TreeException):
  status=265388083
  message="Node is already ON"
  msgnam="ALREADY_ON"



class TreeALREADY_OPEN(TreeException):
  status=265388091
  message="Tree is already OPEN"
  msgnam="ALREADY_OPEN"



class TreeALREADY_THERE(TreeException):
  status=265388168
  message="Node is already in the tree"
  msgnam="ALREADY_THERE"



class TreeBADRECORD(TreeException):
  status=265388218
  message="Data corrupted: cannot read record"
  msgnam="BADRECORD"



class TreeBOTH_OFF(TreeException):
  status=265388184
  message="Both this node and its parent are off"
  msgnam="BOTH_OFF"



class TreeBUFFEROVF(TreeException):
  status=265388306
  message="Output buffer overflow"
  msgnam="BUFFEROVF"



class TreeCONGLOMFULL(TreeException):
  status=265388322
  message="Current conglomerate is full"
  msgnam="CONGLOMFULL"



class TreeCONGLOM_NOT_FULL(TreeException):
  status=265388330
  message="Current conglomerate is not yet full"
  msgnam="CONGLOM_NOT_FULL"



class TreeCONTINUING(TreeException):
  status=265390435
  message="Operation continuing: note following error"
  msgnam="CONTINUING"



class TreeDUPTAG(TreeException):
  status=265388234
  message="Tag name already in use"
  msgnam="DUPTAG"



class TreeEDITTING(TreeException):
  status=265388434
  message="Tree file open for edit: operation not permitted"
  msgnam="EDITTING"



class TreeILLEGAL_ITEM(TreeException):
  status=265388298
  message="Invalid item code or part number specified"
  msgnam="ILLEGAL_ITEM"



class TreeILLPAGCNT(TreeException):
  status=265388242
  message="Illegal page count, error mapping tree file"
  msgnam="ILLPAGCNT"



class TreeINVDFFCLASS(TreeException):
  status=265388346
  message="Invalid data fmt: only CLASS_S can have data in NCI"
  msgnam="INVDFFCLASS"



class TreeINVDTPUSG(TreeException):
  status=265388426
  message="Attempt to store datatype which conflicts with the designated usage of this node"
  msgnam="INVDTPUSG"



class TreeINVPATH(TreeException):
  status=265388290
  message="Invalid tree pathname specified"
  msgnam="INVPATH"



class TreeINVRECTYP(TreeException):
  status=265388354
  message="Record type invalid for requested operation"
  msgnam="INVRECTYP"



class TreeINVTREE(TreeException):
  status=265388226
  message="Invalid tree identification structure"
  msgnam="INVTREE"



class TreeMAXOPENEDIT(TreeException):
  status=265388250
  message="Too many files open for edit"
  msgnam="MAXOPENEDIT"



class TreeNEW(TreeException):
  status=265388059
  message="New tree created"
  msgnam="NEW"



class TreeNMN(TreeException):
  status=265388128
  message="No More Nodes"
  msgnam="NMN"



class TreeNMT(TreeException):
  status=265388136
  message="No More Tags"
  msgnam="NMT"



class TreeNNF(TreeException):
  status=265388144
  message="Node Not Found"
  msgnam="NNF"



class TreeNODATA(TreeException):
  status=265388258
  message="No data available for this node"
  msgnam="NODATA"



class TreeNODNAMLEN(TreeException):
  status=265388362
  message="Node name too long (12 chars max)"
  msgnam="NODNAMLEN"



class TreeNOEDIT(TreeException):
  status=265388274
  message="Tree file is not open for edit"
  msgnam="NOEDIT"



class TreeNOLOG(TreeException):
  status=265388458
  message="Experiment pathname (xxx_path) not defined"
  msgnam="NOLOG"



class TreeNOMETHOD(TreeException):
  status=265388208
  message="Method not available for this object"
  msgnam="NOMETHOD"



class TreeNOOVERWRITE(TreeException):
  status=265388418
  message="Write-once node: overwrite not permitted"
  msgnam="NOOVERWRITE"



class TreeNORMAL(TreeException):
  status=265388041
  message="Normal successful completion"
  msgnam="NORMAL"



class TreeNOTALLSUBS(TreeException):
  status=265388067
  message="Main tree opened but not all subtrees found/or connected"
  msgnam="NOTALLSUBS"



class TreeNOTCHILDLESS(TreeException):
  status=265388282
  message="Node must be childless to become subtree reference"
  msgnam="NOTCHILDLESS"



class TreeNOT_IN_LIST(TreeException):
  status=265388482
  message="Tree being opened was not in the list"
  msgnam="NOT_IN_LIST"



class TreeNOTMEMBERLESS(TreeException):
  status=265388402
  message="Subtree reference can not have members"
  msgnam="NOTMEMBERLESS"



class TreeNOTOPEN(TreeException):
  status=265388266
  message="No tree file currently open"
  msgnam="NOTOPEN"



class TreeNOTSON(TreeException):
  status=265388410
  message="Subtree reference cannot be a member"
  msgnam="NOTSON"



class TreeNOT_CONGLOM(TreeException):
  status=265388386
  message="Head node of conglomerate does not contain a DTYPE_CONGLOM record"
  msgnam="NOT_CONGLOM"



class TreeNOT_OPEN(TreeException):
  status=265388200
  message="Tree not currently open"
  msgnam="NOT_OPEN"



class TreeNOWRITEMODEL(TreeException):
  status=265388442
  message="Data for this node can not be written into the MODEL file"
  msgnam="NOWRITEMODEL"



class TreeNOWRITESHOT(TreeException):
  status=265388450
  message="Data for this node can not be written into the SHOT file"
  msgnam="NOWRITESHOT"



class TreeNO_CONTEXT(TreeException):
  status=265388099
  message="There is no active search to end"
  msgnam="NO_CONTEXT"



class TreeOFF(TreeException):
  status=265388192
  message="Node is OFF"
  msgnam="OFF"



class TreeON(TreeException):
  status=265388107
  message="Node is ON"
  msgnam="ON"



class TreeOPEN(TreeException):
  status=265388115
  message="Tree is OPEN (no edit)"
  msgnam="OPEN"



class TreeOPEN_EDIT(TreeException):
  status=265388123
  message="Tree is OPEN for edit"
  msgnam="OPEN_EDIT"



class TreePARENT_OFF(TreeException):
  status=265388176
  message="Parent of this node is OFF"
  msgnam="PARENT_OFF"



class TreeREADERR(TreeException):
  status=265388474
  message="Error reading record for node"
  msgnam="READERR"



class TreeREADONLY(TreeException):
  status=265388466
  message="Tree was opened with readonly access"
  msgnam="READONLY"



class TreeRESOLVED(TreeException):
  status=265388049
  message="Indirect reference successfully resolved"
  msgnam="RESOLVED"



class TreeSUCCESS(TreeException):
  status=265389633
  message="Operation successful"
  msgnam="SUCCESS"



class TreeTAGNAMLEN(TreeException):
  status=265388370
  message="Tagname too long (max 24 chars)"
  msgnam="TAGNAMLEN"



class TreeTNF(TreeException):
  status=265388152
  message="Tag Not Found"
  msgnam="TNF"



class TreeTREENF(TreeException):
  status=265388160
  message="Tree Not Found"
  msgnam="TREENF"



class TreeUNRESOLVED(TreeException):
  status=265388338
  message="Not an indirect node reference: No action taken"
  msgnam="UNRESOLVED"



class TreeUNSPRTCLASS(TreeException):
  status=265388314
  message="Unsupported descriptor class"
  msgnam="UNSPRTCLASS"



class TreeUNSUPARRDTYPE(TreeException):
  status=265388394
  message="Complex data types not supported as members of arrays"
  msgnam="UNSUPARRDTYPE"



class TreeWRITEFIRST(TreeException):
  status=265388378
  message="Tree has been modified:  write or quit first"
  msgnam="WRITEFIRST"



class TreeFAILURE(TreeException):
  status=265392034
  message="Operation NOT successful"
  msgnam="FAILURE"



class TreeLOCK_FAILURE(TreeException):
  status=265392050
  message="Error locking file, perhaps NFSLOCKING not enabled on this system"
  msgnam="LOCK_FAILURE"



class TreeFILE_NOT_FOUND(TreeException):
  status=265392042
  message="File or Directory Not Found"
  msgnam="FILE_NOT_FOUND"



class TreeCANCEL(TreeException):
  status=265391232
  message="User canceled operation"
  msgnam="CANCEL"



class TreeNOSEGMENTS(TreeException):
  status=265392058
  message="No segments exist in this node"
  msgnam="NOSEGMENTS"



class TreeINVDTYPE(TreeException):
  status=265392066
  message="Invalid datatype for data segment"
  msgnam="INVDTYPE"



class TreeINVSHAPE(TreeException):
  status=265392074
  message="Invalid shape for this data segment"
  msgnam="INVSHAPE"



class TreeINVSHOT(TreeException):
  status=265392090
  message="Invalid shot number - must be -1 (model), 0 (current), or Positive"
  msgnam="INVSHOT"



class TreeINVTAG(TreeException):
  status=265392106
  message="Invalid tagname - must begin with alpha followed by 0-22 alphanumeric or underscores"
  msgnam="INVTAG"



class TreeNOPATH(TreeException):
  status=265392114
  message="No 'treename'_path environment variable defined. Cannot locate tree files."
  msgnam="NOPATH"



class TreeTREEFILEREADERR(TreeException):
  status=265392122
  message="Error reading in tree file contents."
  msgnam="TREEFILEREADERR"



class TreeMEMERR(TreeException):
  status=265392130
  message="Memory allocation error."
  msgnam="MEMERR"



class TreeNOCURRENT(TreeException):
  status=265392138
  message="No current shot number set for this tree."
  msgnam="NOCURRENT"



class TreeFOPENW(TreeException):
  status=265392146
  message="Error opening file for read-write."
  msgnam="FOPENW"



class TreeFOPENR(TreeException):
  status=265392154
  message="Error opening file read-only."
  msgnam="FOPENR"



class TreeFCREATE(TreeException):
  status=265392162
  message="Error creating new file."
  msgnam="FCREATE"



class TreeCONNECTFAIL(TreeException):
  status=265392170
  message="Error connecting to remote server."
  msgnam="CONNECTFAIL"



class TreeNCIWRITE(TreeException):
  status=265392178
  message="Error writing node characterisitics to file."
  msgnam="NCIWRITE"



class TreeDELFAIL(TreeException):
  status=265392186
  message="Error deleting file."
  msgnam="DELFAIL"



class TreeRENFAIL(TreeException):
  status=265392194
  message="Error renaming file."
  msgnam="RENFAIL"



class TreeEMPTY(TreeException):
  status=265392200
  message="Empty string provided."
  msgnam="EMPTY"



class TreePARSEERR(TreeException):
  status=265392210
  message="Invalid node search string."
  msgnam="PARSEERR"



class TreeNCIREAD(TreeException):
  status=265392218
  message="Error reading node characteristics from file."
  msgnam="NCIREAD"



class TreeNOVERSION(TreeException):
  status=265392226
  message="No version available."
  msgnam="NOVERSION"



class TreeDFREAD(TreeException):
  status=265392234
  message="Error reading from datafile."
  msgnam="DFREAD"



class TreeCLOSEERR(TreeException):
  status=265392242
  message="Error closing temporary tree file."
  msgnam="CLOSEERR"



class TreeMOVEERROR(TreeException):
  status=265392250
  message="Error replacing original treefile with new one."
  msgnam="MOVEERROR"



class TreeOPENEDITERR(TreeException):
  status=265392258
  message="Error reopening new treefile for write access."
  msgnam="OPENEDITERR"



class TreeREADONLY_TREE(TreeException):
  status=265392266
  message="Tree is marked as readonly. No write operations permitted."
  msgnam="READONLY_TREE"

