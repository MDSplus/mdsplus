#ifndef CTS_P_H
#define CTS_P_H

extern int remove_entry(int dbType, int index);
extern int find_crate(char *wild, char **crate, void **ctx);
extern void find_crate_end(void **ctx);

#endif
