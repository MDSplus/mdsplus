/*  CMS REPLACEMENT HISTORY, Element TDI$$OPC_DEF.C */
/*  *6    27-JUN-1996 09:10:25 TWF "Port to Unix/Windows" */
/*  *5    25-JUN-1996 08:44:12 TWF "Port to Unix/Windows" */
/*  *4    18-OCT-1995 11:58:53 TWF "make all includes the same case" */
/*  *3     2-NOV-1994 08:51:05 TWF "Change extern model" */
/*  *2    28-OCT-1994 16:23:53 TWF "split out opc$_xx and opc$xx" */
/*  *1    25-OCT-1994 08:22:42 TWF "Define Opc_function and Opcfunction symbols" */
/*  CMS REPLACEMENT HISTORY, Element TDI$$OPC_DEF.C */
#define COM
#define OPC(name,builtin,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11) const unsigned short Opc##name = __LINE__ - 25;
#include "opcbuiltins.h"
#undef OPC

