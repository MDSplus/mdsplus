#ifndef CTS_P_H
#define CTS_P_H
#include <mdsplus/mdsconfig.h>

extern EXPORT int remove_entry(int dbType, int index);
extern EXPORT int find_crate(char *wild, char **crate, void **ctx);
extern EXPORT void find_crate_end(void **ctx);

#endif
