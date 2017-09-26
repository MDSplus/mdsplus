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
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <stdio.h>
#include <string.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <stdlib.h>
#include <strroutines.h>
#include <libroutines.h>
#include "paragon_rpt_gen.h"

extern int PARAGON_FTP_COPY(char *report_in, struct descriptor *report_out, int *isftp);
extern int PARAGON_FTP_DELETE(char *report, int delete);

#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;

/*
int paragon_rpt___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct *setup)
{
  static struct descriptor_d rpt_name = {0,DTYPE_T,CLASS_D,0};
  static DESCRIPTOR(zero,"\0");
  static EMPTYXD(text);
  static EMPTYXD(analog);
  static EMPTYXD(digital);
  static EMPTYXD(text_names);
  static EMPTYXD(analog_names);
  static EMPTYXD(digital_names);
  static DESCRIPTOR_SIGNAL_1(analog_signal, &analog, 0, &analog_names);
  static DESCRIPTOR_SIGNAL_1(digital_signal, &digital, 0, &digital_names);
  static DESCRIPTOR_SIGNAL_1(text_signal, &text, 0, &text_names);
  char line[256];
  FILE *file;
  int isftp;
  int status = PARAGON_FTP_COPY(setup->report_name,(struct descriptor *)&rpt_name,&isftp);
  if (!(status & 1))
  {
    return(status);
  }
  file = fopen(rpt_name.pointer,"r+");
  if (file == 0) return 0;
  while(fgets(line,255,file))
  {
    if (line[0] == '|')
    {
      if (line[1] != '!')
      {
        char name[64];
        char type[3];
        char value[256];
        sscanf(&line[1],"%63s %2s %255s",name,type,value);
        if (strlen(name) && (strlen(type)==1) && strlen(value))
        {
          static struct descriptor name_d = {0,DTYPE_T,CLASS_S,0};
          name_d.length = strlen(name);
          name_d.pointer = name;
          switch (type[0])
          {
            case 'A':
                      {
                        static float val;
                        static DESCRIPTOR_FLOAT(val_d,&val);
                        sscanf(value,"%f",&val);
                        if (analog_names.l_length)
                        {
                          TdiVector(&analog_names,&name_d,&analog_names MDS_END_ARG);
                          TdiVector(&analog,&val_d,&analog MDS_END_ARG);
                        }
                        else
                        {
                          MdsCopyDxXd(&name_d,&analog_names);
                          MdsCopyDxXd(&val_d,&analog);
                        }
                      }
                      break;
            case 'D':
                      {
                        static int val;
                        static DESCRIPTOR_LONG(val_d,&val);
                        val = atoi(value);
                        if (digital_names.l_length)
                        {
                          TdiVector(&digital_names,&name_d,&digital_names MDS_END_ARG);
                          TdiVector(&digital,&val_d,&digital MDS_END_ARG);
                        }
                        else
                        {
                          MdsCopyDxXd(&name_d,&digital_names);
                          MdsCopyDxXd(&val_d,&digital);
                        }
                      }
                      break;
            case 'T':
                      sscanf(&line[1],"%63s %2s %255[^|\n]",name,type,value);
                      {
			unsigned short len;
                        static struct descriptor val_d = {0, DTYPE_T, CLASS_S, 0};
                        static struct descriptor trimmed = {0, DTYPE_T, CLASS_D, 0};
                        val_d.length = strlen(value);
                        val_d.pointer = value;
                        StrTrim(&trimmed,&val_d,&len);
                        if (text_names.l_length)
                        {
                          TdiVector(&text_names,&name_d,&text_names MDS_END_ARG);
                          TdiVector(&text,&trimmed,&text MDS_END_ARG);
                        }
                        else
                        {
                          MdsCopyDxXd(&name_d,&text_names);
                          MdsCopyDxXd(&trimmed,&text);
                        }
                        StrFree1Dx(&trimmed);
                      }
                      break;
            default: printf("Cannot parse report line: %s",line);
          }
        }
        else if (strcmp(name,"DATE:"))
          printf("Cannot parse report line: %s",line);
      }
    }
  }
  fclose(file);
  if (isftp)
    PARAGON_FTP_DELETE(setup->report_name,0);
  if (analog_names.l_length)
  {
    int analog_nid = setup->head_nid + PARAGON_RPT_N_ANALOG;
    TreePutRecord(analog_nid,(struct descriptor *)&analog_signal,0);
    MdsFree1Dx(&analog_names,0);
    MdsFree1Dx(&analog,0);
  }
  if (digital_names.l_length)
  {
    int digital_nid = setup->head_nid + PARAGON_RPT_N_DIGITAL;
    TreePutRecord(digital_nid,(struct descriptor *)&digital_signal,0);
    MdsFree1Dx(&digital_names,0);
    MdsFree1Dx(&digital,0);
  }
  if (text_names.l_length)
  {
    int text_nid = setup->head_nid + PARAGON_RPT_N_TEXT;
    TreePutRecord(text_nid,(struct descriptor *)&text_signal,0);
    MdsFree1Dx(&text_names,0);
    MdsFree1Dx(&text,0);
  }
  return TreeNORMAL;
}
*/
