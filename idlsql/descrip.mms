all : PROCEDURES, -
      mds$root:[syslib]dblib.exe, -
      mds$root:[syslib]idlsql_both.exe, -
      mds$root:[syslib]idlsql_syb.exe, -
      mds$root:[syslib]idlsql_rdb.exe
	@- write sys$output "DBLIB, IDLSQL_BOTH, IDLSQL_SYB and IDLSQL_RDB are up to date"


.ifdef AXP
SQL_MODULE = MODULE_WITH_STATUS.OBJ$(AXP),SUB_MODULE.OBJ$(AXP)
.else
SQL_MODULE = SQL_MODULE.OBJ$(AXP)
.endif

.IFDEF AXP
mds$root:[syslib]dblib.exe : dblib.opt_shared$(AXP)

.else
mds$root:[syslib]dblib.exe : SYBASE_DEVICE:[SYBASE.LIB]SYB_DBSHR.EXE
	copy $(MMS$SOURCE) $(MMS$TARGET)
.ENDIF


mds$root:[syslib]idlsql_both.exe : idlsql_both.opt_shared$(AXP), -
				    idlsql_both.olb$(axp)(idlsql_both.obj$(AXP))

idlsql_both.obj$(axp) : idlsql_both.c
	$(CC) /noopt/deb $(CCFLAGS) /inc=(idl_dir:[external])/float=d_float $(MMS$SOURCE) /obj=$(MMS$TARGET_NAME).obj$(axp)_debug
	$(CC) $(CCFLAGS) /inc=(idl_dir:[external])/float=d_float $(MMS$SOURCE) /obj=$(MMS$TARGET)


mds$root:[syslib]idlsql_syb.exe : idlsql_syb.opt_shared$(axp), -
                                  idlsql_syb.olb$(axp)( -
                                    DYNAMIC_SYB=DYNAMIC_SYB.obj$(AXP), -
                                    USER_SYB=USER_SYB.obj$(AXP), -
                                    IDLSQL_SYB=IDLSQL_SYB.obj$(AXP), -
                                    cvtconvertfloat=cvtconvertfloat.obj$(AXP), -
				    IEEECVT=IEEECVT.obj$(AXP))


DYNAMIC_SYB.OBJ$(AXP) : DYNAMIC_SYB.C
	$(CC) /noopt/deb $(CCFLAGS) /inc=(SYBASE_DEVICE:[SYBASE.INCLUDE])/float=d_float $(MMS$SOURCE) /obj=$(MMS$TARGET_NAME).obj$(axp)_debug
	$(CC) $(CCFLAGS) /inc=(SYBASE_DEVICE:[SYBASE.INCLUDE])/float=d_float $(MMS$SOURCE) /obj=$(MMS$TARGET)

IDLSQL_SYB.OBJ$(AXP) : IDLSQL_SYB.C
	$(CC) /noopt/deb $(CCFLAGS) /inc=(sybase_device:[sybase.include],idl_dir:[external])/float=d_float $(MMS$SOURCE) /obj=$(MMS$TARGET_NAME).obj$(axp)_debug
	$(CC) $(CCFLAGS) /inc=(SYBASE_DEVICE:[SYBASE.INCLUDE],idl_dir:[external])/float=d_float $(MMS$SOURCE) /obj=$(MMS$TARGET)


USER_SYB.OBJ$(AXP) : USER_SYB.C
	$(CC) /noopt/deb $(CCFLAGS) /inc=(sybase_device:[SYBASE.INCLUDE])/float=d_float $(MMS$SOURCE) /obj=$(MMS$TARGET_NAME).obj$(axp)_debug
	$(CC) $(CCFLAGS) /inc=(SYBASE_DEVICE:[SYBASE.INCLUDE])/float=d_float $(MMS$SOURCE) /obj=$(MMS$TARGET)

IEEECVT.OBJ$(AXP) : IEEECVT.C
	$(CC) /noopt/deb $(CCFLAGS) /inc=(idl_dir:[external])/float=d_float $(MMS$SOURCE) /obj=$(MMS$TARGET_NAME).obj$(axp)_debug
	$(CC) $(CCFLAGS) /inc=(idl_dir:[external])/float=d_float $(MMS$SOURCE) /obj=$(MMS$TARGET)

mds$root:[syslib]idlsql_rdb.exe : idlsql_rdb.opt_shared$(axp), -
                                  idlsql_rdb.olb$(axp)( -
                                    DYNAMIC_RDB=DYNAMIC_RDB.obj$(AXP), -
                                    USER_RDB=USER_RDB.obj$(AXP), -
                                    IDLSQL_RDB=IDLSQL_RDB.obj$(AXP), -
				    IEEECVT=IEEECVT.obj$(AXP), -
				    $(SQL_MODULE))


IDLSQL_RDB.OBJ$(AXP) : IDLSQL_RDB.C
	$(CC) /noopt/deb $(CCFLAGS) /inc=(idl_dir:[external])/float=d_float $(MMS$SOURCE) /obj=$(MMS$TARGET_NAME).obj$(axp)_debug
	$(CC) $(CCFLAGS) /inc=(idl_dir:[external])/float=d_float $(MMS$SOURCE) /obj=$(MMS$TARGET)



PROCEDURES : -
	MDS$ROOT:[000000]IDL.DIR

	@ write sys$output "IDL procedures up-to-date"

