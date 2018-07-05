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
#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <mdsdescrip.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <tdishr.h>

int GetSupportedDevices(char ***devnames, int *number)
{
  EMPTYXD(dev_list);
  static DESCRIPTOR(expr, "_devs = MdsDevices()");
  int status;
  *number=0;
  *devnames=0;
  status = TdiExecute((struct descriptor *)&expr, &dev_list MDS_END_ARG);
  if (status & 1) {
    int i;
    struct descriptor_a *a_ptr = (struct descriptor_a *)dev_list.pointer;
    *number = a_ptr->arsize/a_ptr->length/2;
    *devnames = (char **)malloc(*number * sizeof(char **));
    for (i = 0; i < *number; i++) {
      char *devname;
      (*devnames)[i] = devname = strndup((const char *)a_ptr->pointer + (i * 2 * a_ptr->length),(size_t)a_ptr->length);
      while(strlen(devname) > 0 && isspace(devname[strlen(devname)-1]))
	devname[strlen(devname)-1]='\0';
    }
  }
  MdsFree1Dx(&dev_list,0);
  return status;
}
