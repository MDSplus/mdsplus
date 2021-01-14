#ifdef __APPLE__
#include "fortran_aliases_mac.h"
#else
#define ALIAS(aname, name) EXPORT int aname() __attribute__((alias(name)));
ALIAS(descr_, "descr")
ALIAS(DESCR, "descr")
ALIAS(descr2_, "descr2")
ALIAS(DESCR2, "descr")
ALIAS(mdsconnect_, "MdsConnect")
ALIAS(mdsconnectr_, "MdsConnectR")
ALIAS(mdsconnect, "MdsConnect")
ALIAS(MDSCONNECT, "MdsConnect")
ALIAS(mdsclose_, "MdsClose")
ALIAS(mdscloser_, "MdsCloseR")
ALIAS(mdsclose, "MdsClose")
ALIAS(MDSCLOSE, "MdsClose")
ALIAS(mdsdisconnect_, "MdsDisconnect")
ALIAS(mdsdisconnectr_, "MdsDisconnectR")
ALIAS(mdsdisconnect, "MdsDisconnect")
ALIAS(MDSDISCONNECT, "MdsDisconnect")
ALIAS(mdsopen_, "MdsOpen")
ALIAS(mdsopenr_, "MdsOpenR")
ALIAS(mdsopen, "MdsOpen")
ALIAS(MDSOPEN, "MdsOpen")
ALIAS(mdssetdefault_, "MdsSetDefault")
ALIAS(mdssetdefaultr_, "MdsSetDefaultR")
ALIAS(mdssetdefault, "MdsSetDefault")
ALIAS(MDSSETDEFAULT, "MdsSetDefault")
ALIAS(mdssetsocket_, "MdsSetSocket")
ALIAS(mdssetsocket, "MdsSetSocket")
ALIAS(MDSSETSOCKET, "MdsSetSocket")
ALIAS(mdsput_, "MdsPut")
ALIAS(mdsputr_, "MdsPutR")
ALIAS(mdsput, "MdsPut")
ALIAS(MDSPUT, "MdsPut")
ALIAS(mdsvalue_, "MdsValue")
ALIAS(mdsvaluer_, "MdsValueR")
ALIAS(mdsvalue, "MdsValue")
ALIAS(MDSVALUE, "MdsValue")
ALIAS(mdsput2_, "MdsPut2")
ALIAS(mdsput2r_, "MdsPut2R")
ALIAS(mdsput2, "MdsPut2")
ALIAS(MDSPUT2, "MdsPut2")
ALIAS(mdsvalue2_, "MdsValue2")
ALIAS(mdsvalue2r_, "MdsValue2R")
ALIAS(mdsvalue2, "MdsValue2")
ALIAS(MDSVALUE2, "MdsValue2")
#endif
