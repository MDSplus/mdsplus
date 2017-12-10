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
#include "mdsip_connections.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <libroutines.h>


////////////////////////////////////////////////////////////////////////////////
//  LoadIo  ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

IoRoutines *LoadIo(char *protocol_in){
  IoRoutines *(*rtn) ();
  if (protocol_in == 0)
    protocol_in = "TCP";
  char *protocol = strcpy((char *)malloc(strlen(protocol_in) + 1), protocol_in);
  size_t i;
  for (i = 0; i < strlen(protocol) ; i++)
    protocol[i] = toupper(protocol[i]);
  char* image = strcpy((char *)malloc(strlen(protocol) + 36), "MdsIp");
  strcat(image, protocol);
  int ok = LibFindImageSymbol_C(image, "Io", (void**)&rtn);
  free(image);
  if (ok && rtn) {
    free(protocol);
    return rtn();
  } else {
    fprintf(stderr, "Protocol %s is not supported\n", protocol);
    free(protocol);
    return NULL;
  }
}
