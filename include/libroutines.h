#ifdef __VMS
#include <lib$routines.h>
#define LibAddx lib$addx
#define LibCallg lib$callg
#define LibConvertDateString lib$convert_date_string
#define LibCreateVmZone lib$create_vm_zone
#define LibEmul lib$emul
#define LibEstablish lib$establish
#define LibFindFile lib$find_file
#define LibFindFileEnd lib$find_file_end
#define LibFindImageSymbol lib$find_image_symbol
#define LibFindVmZone lib$find_vm_zone
#define LibFreeVm lib$free_vm
#define LibGetDvi lib$getdvi
#define LibGetVm lib$get_vm
#define LibInsertTree lib$insert_tree
#define LibLookupTree lib$lookup_tree
#define LibResetVmZone lib$reset_vm_zone
#define LibSFree1Dd lib$sfree1_dd
#define LibShowVmZone lib$show_vm_zone
#define LibSigToRet lib$sig_to_ret
#define LibSpawn lib$spawn
#define LibSubx lib$subx
#define LibSysAscTim lib$sys_asctim
#define LibTraverseTree lib$traverse_tree
#define LibWait lib$wait

#else
#include <config.h>

EXPORT extern int LibAddx();
EXPORT extern void  *LibCallg();
EXPORT extern int LibConvertDateString();
EXPORT extern int LibCreateVmZone();
EXPORT extern int LibDeleteVmZone();
EXPORT extern int LibEmul();
EXPORT extern int LibEstablish();
EXPORT extern int LibFindFile();
EXPORT extern int LibFindFileEnd();
EXPORT extern int LibFindFileRecurseCaseBlind();
EXPORT extern int LibFindImageSymbol();
EXPORT extern char *LibFindImageSymbolErrString();
EXPORT extern char *LibFindImageSymbolErrStr();
EXPORT extern int LibFindVmZone();
EXPORT extern int LibFreeVm();
EXPORT extern int LibGetVm();
EXPORT extern int libgetvm_();
EXPORT extern int libfreevm_();
EXPORT extern int LibInsertTree();
EXPORT extern int LibLookupTree();
EXPORT extern int LibResetVmZone();
EXPORT extern int LibSFree1Dd();
EXPORT extern int LibShowVmZone();
EXPORT extern int LibSigToRet();
EXPORT extern int LibSpawn();
EXPORT extern int LibSubx();
EXPORT extern int LibSysAscTim();
EXPORT extern int LibTraverseTree();
EXPORT extern int LibWait();
EXPORT extern int LibTimeToVMSTime();
EXPORT extern int libffs(int *position, int *size, char *base, int *find_position);
#endif
