
########################################################
# This module was generated using mdsshr/gen_device.py
# To add new status messages modify:
#     tdishr_messages.xml
# and then in mdsshr do:
#     python gen_devices.py
########################################################

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

MDSplusException=_mimport('__init__').MDSplusException


class TdiException(MDSplusException):
  fac="Tdi"


class TdiBREAK(TdiException):
  status=265519112
  message="BREAK was not in DO FOR SWITCH or WHILE"
  msgnam="BREAK"



class TdiCASE(TdiException):
  status=265519120
  message="CASE was not in SWITCH statement"
  msgnam="CASE"



class TdiCONTINUE(TdiException):
  status=265519128
  message="CONTINUE was not in DO FOR or WHILE"
  msgnam="CONTINUE"



class TdiEXTRANEOUS(TdiException):
  status=265519136
  message="Some characters were unused, bad number maybe"
  msgnam="EXTRANEOUS"



class TdiRETURN(TdiException):
  status=265519144
  message="Extraneous RETURN statement, not from a FUN"
  msgnam="RETURN"



class TdiABORT(TdiException):
  status=265519154
  message="Program requested abort"
  msgnam="ABORT"



class TdiBAD_INDEX(TdiException):
  status=265519162
  message="Index or subscript is too small or too big"
  msgnam="BAD_INDEX"



class TdiBOMB(TdiException):
  status=265519170
  message="Bad punctuation, could not compile the text"
  msgnam="BOMB"



class TdiEXTRA_ARG(TdiException):
  status=265519178
  message="Too many arguments for function, watch commas"
  msgnam="EXTRA_ARG"



class TdiGOTO(TdiException):
  status=265519186
  message="GOTO target label not found"
  msgnam="GOTO"



class TdiINVCLADSC(TdiException):
  status=265519194
  message="Storage class not valid, must be scalar or array"
  msgnam="INVCLADSC"



class TdiINVCLADTY(TdiException):
  status=265519202
  message="Invalid mixture of storage class and data type"
  msgnam="INVCLADTY"



class TdiINVDTYDSC(TdiException):
  status=265519210
  message="Storage data type is not valid"
  msgnam="INVDTYDSC"



class TdiINV_OPC(TdiException):
  status=265519218
  message="Invalid operator code in a function"
  msgnam="INV_OPC"



class TdiINV_SIZE(TdiException):
  status=265519226
  message="Number of elements does not match declaration"
  msgnam="INV_SIZE"



class TdiMISMATCH(TdiException):
  status=265519234
  message="Shape of arguments does not match"
  msgnam="MISMATCH"



class TdiMISS_ARG(TdiException):
  status=265519242
  message="Missing argument is required for function"
  msgnam="MISS_ARG"



class TdiNDIM_OVER(TdiException):
  status=265519250
  message="Number of dimensions is over the allowed 8"
  msgnam="NDIM_OVER"



class TdiNO_CMPLX(TdiException):
  status=265519258
  message="There are no complex forms of this function"
  msgnam="NO_CMPLX"



class TdiNO_OPC(TdiException):
  status=265519266
  message="No support for this function, today"
  msgnam="NO_OPC"



class TdiNO_OUTPTR(TdiException):
  status=265519274
  message="An output pointer is required"
  msgnam="NO_OUTPTR"



class TdiNO_SELF_PTR(TdiException):
  status=265519282
  message="No $VALUE is defined for signal or validation"
  msgnam="NO_SELF_PTR"



class TdiNOT_NUMBER(TdiException):
  status=265519290
  message="Value is not a scalar number and must be"
  msgnam="NOT_NUMBER"



class TdiNULL_PTR(TdiException):
  status=265519298
  message="Null pointer where value needed"
  msgnam="NULL_PTR"



class TdiRECURSIVE(TdiException):
  status=265519306
  message="Overly recursive function, calls itself maybe"
  msgnam="RECURSIVE"



class TdiSIG_DIM(TdiException):
  status=265519314
  message="Signal dimension does not match data shape"
  msgnam="SIG_DIM"



class TdiSYNTAX(TdiException):
  status=265519322
  message="Bad punctuation or misspelled word or number"
  msgnam="SYNTAX"



class TdiTOO_BIG(TdiException):
  status=265519330
  message="Conversion of number lost significant digits"
  msgnam="TOO_BIG"



class TdiUNBALANCE(TdiException):
  status=265519338
  message="Unbalanced () [] {} '' " " or /**/"
  msgnam="UNBALANCE"



class TdiUNKNOWN_VAR(TdiException):
  status=265519346
  message="Unknown/undefined variable name"
  msgnam="UNKNOWN_VAR"



class TdiSTRTOOLON(TdiException):
  status=265519356
  message="string is too long (greater than 65535)"
  msgnam="STRTOOLON"



class TdiTIMEOUT(TdiException):
  status=265519364
  message="task did not complete in alotted time"
  msgnam="TIMEOUT"

