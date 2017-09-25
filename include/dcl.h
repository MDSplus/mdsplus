/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

#include <mdsdescrip.h>
extern int mdsdclAddCommands(const char *name_in, char **error);
extern char *mdsdclGetPrompt();
extern char *mdsdclGetHistoryFile();
extern void *mdsdclSetOutputRtn(void (*rtn)());
extern void *mdsdclSetErrorRtn(void (*rtn)());
extern void mdsdclFlushOutput(char *output);
extern void mdsdclFlushError(char *error);
extern int mdsdcl_do_command(char const *command);
extern int mdsdcl_do_command_dsc(char const *command, struct descriptor_xd *error_dsc,
					struct descriptor_xd *output_dsc);
extern int mdsdcl_do_command_extra_args(char const *command, char **prompt, char **output,
                                        char **error, char *(*getline) (), void *getlineinfo);
extern int cli_get_value(void *ctx, const char *name, char **value);
extern int cli_present(void *ctx, const char *name);

