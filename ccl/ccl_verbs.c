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
#include <stdio.h>
#include <stdlib.h>
#include <dcl.h>
#include <string.h>
#include <camshr.h>
#include <mdsshr.h>

static int ParseQualifiers();
static int CheckErrors();
typedef struct {
  unsigned short condition;
  unsigned short bytcnt;
  unsigned int devstat;
} IOSB;

static char *Name;
static char *DefName;
static int A = 0;
static int F = 0;
static int Mem = 24;
static void *D = 0;
static int Count;
static IOSB iosb;
static int LastStatus;
static int Xrequired = 1;
static int Qrequired = 0;

#define not_implemented(a) \
    EXPORT int a(){printf("\n==>  %s : This command is not implemented on this platform\n\n",#a);return 0;}

#define multi_io(ccl_name,cam_name) EXPORT int ccl_name(void *ctx, char **error, char **output) \
{\
  int status = ParseQualifiers(ctx, error, output);				\
  if (status & 1) status = cam_name(Name,A,F,Count,D,Mem,(unsigned short *)&iosb); \
  return CheckErrors(status,(unsigned short *)&iosb, error, output);	\
}

#define single_io(ccl_name,cam_name) EXPORT int ccl_name(void *ctx, char **error, char **output) \
{\
  int status = ParseQualifiers(ctx, error);				\
  if (status & 1) status = cam_name(Name,A,F,D,Mem,(unsigned short *)&iosb); \
  return CheckErrors(status,(unsigned short *)&iosb, error, output);	\
}

single_io(ccl_pio, CamPiow)
multi_io(ccl_fstop, CamFStopw)
multi_io(ccl_fqrep, CamFQrepw)
multi_io(ccl_fqstop, CamFQstopw)
multi_io(ccl_qrep, CamQrepw)
multi_io(ccl_qscan, CamQscanw)
multi_io(ccl_qstop, CamQstopw)

int ccl_set_module();
int ccl_set_xandq();
int ccl_show_data();
int ccl_show_module();
int ccl_show_status();
multi_io(ccl_stop, CamStopw)
    not_implemented(ccl_wdata)

static int ParseQualifiers(void *ctx, char **error)
{
  int *d32;
  short *d16;
  int binary, hex, octal;
  int i;
  char *module = 0;
  char *value = 0;
  if (cli_get_value(ctx,"module",&module)&1) {
    if (Name)
      free(Name);
    Name = module;
  } else {
    if (DefName) {
      if (Name)
	free(Name);
      Name = strdup(DefName);
    }
    else {
      *error = strdup("No module selected\n");
      return 0;
    }
  }
  A = 0;
  if (cli_get_value(ctx,"address", &value) & 1) {
    char *endptr;
    A = strtol(value, &endptr, 0);
    if (*endptr != '\0' || (A < 0) || (A > 15)) {
      *error = malloc(strlen(value)+100);
      sprintf(*error, "Error: invalid /ADDRESS value specified '%s'. Use a number from 0 to 15\n",value);
      free(value);
      return 0;
    }
    free(value);
  }
  F = 0;
  if (cli_get_value(ctx,"function", &value) & 1) {
    char *endptr;
    F = strtol(value, &endptr, 0);
    if (*endptr != '\0' || (F < 0) || (F > 31)) {
      *error = malloc(strlen(value)+100);
      sprintf(*error, "Error: invalid /FUNCTION value specified '%s'. Use a number from 0 to 31\n",value);
      free(value);
      return 0;
    }
    free(value);
  }
  Count = 1;
  if (cli_get_value(ctx,"count", &value) & 1) {
    char *endptr;
    Count = strtol(value, &endptr, 0);
    if (*endptr != '\0' && (Count < 0)) {
      *error = malloc(strlen(value)+100);
      sprintf(*error, "Error: invalid /COUNT value specified '%s'. Use a number > 0.\n",value);
      free(value);
      return 0;
    }
    free(value);
  }
  Mem = 24;
  if (cli_get_value(ctx,"memory", &value) & 1) {
    char *endptr;
    Mem = strtol(value, &endptr, 0);
    if (*endptr != '\0' || ((Mem != 16) && (Mem != 24))) {
      *error = malloc(strlen(value)+100);
      sprintf(*error, "Error: invalid /MEMORY value specified '%s'. Use 16 or 24.\n",value);
      free(value);
      return 0;
    }
    free(value);
  }
  if (D)
    free(D);
  D = malloc(Count * ((Mem == 24) ? 4 : 2));
  d16 = (short *)D;
  d32 = (int *)D;
  binary = cli_present(ctx, "binary") & 1;
  hex = cli_present(ctx, "hex") & 1;
  octal = cli_present(ctx, "octal") & 1;
  for (i = 0; i < Count; i++) {
    char *value = 0;
    int val = 0;
    if (cli_get_value(ctx, "data", &value) & 1) {
      if (binary) {
	size_t j;
	for (j = 0; j < strlen(value); j++) {
	  char v = value[strlen(value) - j - 1];
	  if (v != '0' && v != '1') {
	    *error = malloc(strlen(value) + 200);
	    sprintf(*error,"Error: bad value for /DATA with /BINARY '%s'.\n"
		    "Number must be specified in binary format (all 0's or 1's).\n",value);
	    free(value);
	    return 0;
	  }
	  val = val | ((v - '0') << j);
	}
      } else if (hex) {
	sscanf(value, "%x", &val);
      } else if (octal) {
	sscanf(value, "%o", &val);
      } else {
	sscanf(value, "%d", &val);
      }
      free(value);
    }
    if (Mem == 24)
      d32[i] = val;
    else
      d16[i] = (short)val;
  }
  return 1;
}

static void append(char **target, char *string) {
  if (*target == NULL)
    *target = strdup(string);
  else 
    *target = strcat(realloc(*target,strlen(*target)+strlen(string)+1),string);
}

static int CheckErrors(int status, IOSB * iosb, char **error, char **output)
{
  LastStatus = status;
  if (status & 1) {
    if (Xrequired) {
      int x = CamX((unsigned short *)iosb) & 1;
      if (x && (Xrequired == -1))
	*output = strdup("CAMX: got X=1, expecting X=0\n");
      else if (!x && (Xrequired == 1))
	*output = strdup("CAMNOX: got X=0, expecting X=1\n");
    }
    if (Qrequired) {
      int q = CamQ((unsigned short *)iosb) & 1;
      if (q && (Qrequired == -1))
	*output = strdup("CAMQ: got Q=1, expecting Q=0\n");
      else if (!q && (Qrequired == 1))
	*output = strdup("CAMNOQ: got Q=0, expecting Q=1\n");
    }
  } else {
    if (*error == NULL) {
      char *msg = MdsGetMsg(status);
      *error = malloc(strlen(msg)+100);
      sprintf(*error, "Error detected in CAMAC call, %s\n", msg);
    }
  }
  return status;
}

EXPORT int ccl_set_xandq(void *ctx, char **error __attribute__ ((unused)), char **output __attribute__ ((unused)))
{
  char *value = 0;
  if (cli_get_value(ctx, "x", &value) & 1) {
    if (value[0] == 'N' || value[0] == 'n')
      Xrequired = -1;
    else if (value[0] == 'Y' || value[0] == 'y')
      Xrequired = 1;
    else
      Xrequired = 0;
    free(value);
  }
  if (cli_get_value(ctx, "q", &value) & 1) {
    if (value[0] == 'N' || value[0] == 'n')
      Qrequired = -1;
    else if (value[0] == 'Y' || value[0] == 'y')
      Qrequired = 1;
    else
      Qrequired = 0;
    free(value);
  }
  return 1;
}

EXPORT int ccl_set_module(void *ctx, char **error __attribute__ ((unused)), char **output __attribute__ ((unused)))
{
  int status = cli_get_value(ctx, "name", &DefName) & 1;
  return status;
}

EXPORT int ccl_show_module(void *ctx __attribute__ ((unused)), char **error __attribute__ ((unused)), char **output)
{
  *output = malloc((DefName ? strlen(DefName) : 12) + 100);
  sprintf(*output, "Module set to %s\n", DefName ? DefName : "<undefined>");
  return 1;
}

EXPORT int ccl_show_status(void *ctx __attribute__ ((unused)), char **error __attribute__ ((unused)), char **output)
{
  *output = malloc(100);
  sprintf(*output,"Last status = 0x%x, iosb status = 0x%x, bytcnt = %d, %s, %s\n",
	  LastStatus, iosb.condition, CamBytcnt((unsigned short *)&iosb), (CamX((unsigned short *)&iosb) & 1) ? "X=1" : "X=0",
	  (CamQ((unsigned short *)&iosb) & 1) ? "Q=1" : "Q=0");
  return 1;
}

EXPORT int ccl_show_data(void *ctx, char **error __attribute__ ((unused)), char **output)
{
  int *d32 = (int *)D;
  short *d16 = (short *)D;
  int binary = cli_present(ctx, "binary") & 1;
  int octal = cli_present(ctx, "octal") & 1;
  int hex = cli_present(ctx, "hex") & 1;
  int first = 1;
  int last = 1;
  int i;
  int chars;
  int maxsmps = iosb.bytcnt / ((Mem == 24) ? 4 : 2);
  char *format;
  char outline[256];
  char *value = 0;
  first = (cli_get_value(ctx, "START", &value) & 1) ? atoi(value) : 1;
  if (value)
    free(value);
  value = 0;
  if (first < 1)
    first = 1;
  if (first > maxsmps)
    first = maxsmps;
  last = (cli_get_value(ctx, "END", &value) & 1) ? atoi(value) : first + 99;
  if (value)
    free(value);
  value = 0;
  if (last < first)
    last = first;
  if (last > maxsmps)
    last = maxsmps;
  for (i = first, chars = 0; (first > 0) && (i <= last); i++) {
    if (chars == 0)
      chars = sprintf(outline, "%06d ", i);
    if (octal)
      format = (Mem == 24) ? " %#011o" : " %#06ho";
    else if (hex)
      format = (Mem == 24) ? " %#011x" : " %#06hx";
    else
      format = (Mem == 24) ? " %11d" : " %6hd";
    if (binary) {
      int j, mask, first = 1;
      for (j = Mem - 1; j >= 0; j--) {
	char c;
	mask = 1 << j;
	c = ((Mem == 24) ? d32[i - 1] : d16[i - 1]) & mask ? '1' : '0';
	if (c == '0' && first)
	  continue;
	first = 0;
	outline[chars++] = c;
      }
      outline[chars] = '\0';
    } else
      chars += sprintf(&outline[chars], format, (Mem == 24) ? d32[i - 1] : d16[i - 1]);
    if (chars > 72) {
      outline[chars++]='\n';
      outline[chars]='\0';
      append(output, outline);
      chars = 0;
    }
  }
  if (chars > 0) {
    outline[chars++]='\n';
    outline[chars]='\0';
    append(output, outline);
  }
  return 1;
}

EXPORT int ccl_set_verbose(void *cts __attribute__ ((unused)),
			   char **error __attribute__ ((unused)),
			   char **output __attribute ((unused)))
{
  return CamVerbose(1);
}

EXPORT int ccl_set_noverbose(void *cts __attribute__ ((unused)),
			     char **error __attribute__ ((unused)),
			     char **output __attribute__ ((unused)))
{
  return CamVerbose(0);
}
