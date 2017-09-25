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
#include <mdsdescrip.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mds_stdarg.h>
#include <mdsshr.h>
#include <time.h>


EXPORT int registerListener(char *experiment, char *tree, int shot);
EXPORT void unregisterListener(int listenerId);
EXPORT struct descriptor_xd *getNewSamplesSerializedXd();
extern int TdiDecompile();

static void printDecompiled(struct descriptor *inD)
{
  int status;
  EMPTYXD(out_xd);
  char *buf;

  status = TdiDecompile(inD, &out_xd MDS_END_ARG);
  if (!(status & 1)) {
    printf("%s\n", MdsGetMsg(status));
    return;
  }
  buf = malloc(out_xd.pointer->length + 1);
  memcpy(buf, out_xd.pointer->pointer, out_xd.pointer->length);
  buf[out_xd.pointer->length] = 0;
  out_xd.pointer->pointer[out_xd.pointer->length - 1] = 0;
  printf("%s\n", buf);
  free(buf);
  MdsFree1Dx(&out_xd, 0);
}

int main(int argc, char *argv[])
{
	int id, id1;
	int status;
	struct timespec waitTime;
	struct descriptor_xd *retXd;
	EMPTYXD(xd);

	id = registerListener("2 * test_0", "test", 1);
	id1 = registerListener("10 * build_path('test_1')", "test", 1);
	while(1)
	{
		retXd = getNewSamplesSerializedXd();
		if(!retXd->pointer)
		{
			printf("Error: Null serialized  returned\n");
			exit(0);
		}
		if(retXd->pointer->class != CLASS_A)
		{
			printf("Error: Unexpected serilized class\n");
			exit(0);
		}
		status = MdsSerializeDscIn(retXd->pointer->pointer, &xd);
		if(!(status & 1))
		{
			printf("Error: decompressing result\n");
			exit(0);
		}
		printDecompiled(xd.pointer);
		MdsFree1Dx(&xd, 0);
		MdsFree1Dx(retXd, 0);
		free((char *)retXd);
	}
}

