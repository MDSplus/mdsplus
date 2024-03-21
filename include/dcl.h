#ifndef _DCL_H
#define _DCL_H

#include <mdsdescrip.h>
extern int mdsdclAddCommands(const char *name_in, char **error);
extern char *mdsdclGetPrompt();
extern char *mdsdclGetHistoryFile();
extern void *mdsdclSetOutputRtn(void (*rtn)());
extern void *mdsdclSetErrorRtn(void (*rtn)());
extern void mdsdclFlushOutput(char *output);
extern void mdsdclFlushError(char *error);
extern int mdsdcl_do_command(char const *command);
extern int mdsdcl_do_command_dsc(char const *command,
                                 struct descriptor_xd *error_dsc,
                                 struct descriptor_xd *output_dsc);
extern int _mdsdcl_do_command_dsc(void **ctx, char const *command,
                                  struct descriptor_xd *error_dsc,
                                  struct descriptor_xd *output_dsc);
extern int mdsdcl_do_command_extra_args(char const *command, char **prompt,
                                        char **output, char **error,
                                        char *(*getline)(), void *getlineinfo);
extern int cli_get_value(void *ctx, const char *name, char **value);
extern int cli_present(void *ctx, const char *name);
#endif