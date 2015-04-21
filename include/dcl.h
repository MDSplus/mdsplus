#ifndef DCL_H
#define DCL_H

#include <mdsdcl_messages.h>

extern void *mdsdclSetOutputRtn(void (*rtn)());
extern void *mdsdclSetErrorRtn(void (*rtn)());
extern void mdsdclFlushOutput(char *output);
extern void mdsdclFlushError(char *error);
extern int mdsdcl_do_command(char const *command);
extern int cli_get_value(void *ctx, char *name, char **value);
extern int cli_present(void *ctx, char *name);

#endif
