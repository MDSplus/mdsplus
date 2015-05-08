#ifndef DCL_H
#define DCL_H

#include <config.h>
#include <mdsdcl_messages.h>

EXPORT extern void *mdsdclSetOutputRtn(void (*rtn)());
EXPORT extern void *mdsdclSetErrorRtn(void (*rtn)());
EXPORT extern void mdsdclFlushOutput(char *output);
EXPORT extern void mdsdclFlushError(char *error);
EXPORT extern int mdsdcl_do_command(char const *command);
EXPORT extern int mdsdcl_do_command_extra_args(char const *command, char **prompt, char **output,
                                        char **error, char *(*getline) (), void *getlineinfo);
EXPORT extern int cli_get_value(void *ctx, const char *name, char **value);
EXPORT extern int cli_present(void *ctx, const char *name);

#endif
