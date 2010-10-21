TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
LIBRARY_IOC += mdsplusSup
DBDINC += mdsputRecord
DBDINC += mdsactionRecord
DBDINC += mdsexprRecord
DBD += dbCommon.dbd
DBD += mdsputRecord.dbd
DBD += mdsactionRecord.dbd
DBD += mdsplusSupUtilities.dbd
DBD += devMpSoft.dbd
DBD += devMaSoft.dbd
DBD += devMeSoft.dbd
DBD += devExprSoft.dbd
mdsplusSup_SRCS += devMeSoft.c
mdsplusSup_SRCS += devMpSoft.c
mdsplusSup_SRCS += devMaSoft.c
mdsplusSup_SRCS += devExprSoft.c
mdsplusSup_SRCS += devwftest.c
mdsplusSup_SRCS += mdsactionRecord.c
mdsplusSup_SRCS += mdsexprRecord.c
mdsplusSup_SRCS += mdsputRecord.c
mdsplusSup_SRCS += mdsplusSupUtilities.cpp
mdsplusSup_LIBS += $(EPICS_BASE_IOC_LIBS)
include $(TOP)/configure/RULES
USR_INCLUDES += -I/usr/local/mdsplus/include
#----------------------------------------
#  ADD RULES AFTER THIS LINE

