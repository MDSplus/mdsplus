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

extern int LibAddx();
extern unsigned int LibCallg();
extern int LibConvertDateString();
extern int LibCreateVmZone();
extern int LibEmul();
extern int LibEstablish();
extern int LibFindFile();
extern int LibFindFileEnd();
extern int LibFindFileRecurseCaseBlind();
extern int LibFindImageSymbol();
extern char *LibFindImageSymbolErrString();
extern char *LibFindImageSymbolErrStr();
extern int LibFindVmZone();
extern int LibFreeVm();
extern int LibGetVm();
extern int LibInsertTree();
extern int LibLookupTree();
extern int LibResetVmZone();
extern int LibSFree1Dd();
extern int LibShowVmZone();
extern int LibSigToRet();
extern int LibSpawn();
extern int LibSubx();
extern int LibSysAscTim();
extern int LibTraverseTree();
extern int LibWait();

#endif
