TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#=============================
LIBRARY_IOC += mdsplusSup
DBDINC += mdsexprRecord
DBD += dbCommon.dbd
DBD += mdsplusSupUtilities.dbd
DBD += devMeSoft.dbd
DBD += devExprSoft.dbd
mdsplusSup_SRCS += devMeSoft.c
mdsplusSup_SRCS += devExprSoft.c
mdsplusSup_SRCS += mdsexprRecord.c
mdsplusSup_SRCS += mdsplusSupUtilities.cpp
mdsplusSup_LIBS += $(EPICS_BASE_IOC_LIBS)
include $(TOP)/configure/RULES
USR_INCLUDES += -I/usr/local/mdsplus/include
#----------------------------------------
#  ADD RULES AFTER THIS LINE

