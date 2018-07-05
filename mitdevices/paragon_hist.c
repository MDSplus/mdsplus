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
/*------------------------------------------------------------------------------

  Name:   PARAGON_HIST

  Type:   C function

       Author: Josh Stillerman

  Date:   8-OCT-1992

      Purpose: PARAGON_HIST (device for reading PARAGON HISTORY files)

------------------------------------------------------------------------------
   Copyright (c) 1992
   Property of Massachusetts Institute of Technology, Cambridge MA 02139.
   This program cannot be copied or distributed in any form for non-MIT
   use without specific written approval of MIT Plasma Fusion Center
   Management.
---------------------------------------------------------------------------

  Description:

------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mdsplus/mdsconfig.h>
#include <mdstypes.h>
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <strroutines.h>
#include <libroutines.h>
#include <mdsshr.h>
#include <treeshr.h>
#include <time.h>
#include "paragon_hist_gen.h"
#include "devroutines.h"

EXPORT int PARAGON_FTP_COPY(char *report_in, struct descriptor *report_out, int *isftp);
EXPORT int PARAGON_FTP_DELETE(char *report, int delete);

#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;

#define INITIAL_SIZE 1000
typedef struct _time {
  int lo, hi;
} TIME;

static int Store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup, int sort);

static int ParseHistorian(char *line, struct descriptor *name, float *value, TIME * time);
static void StoreSignal(int nid,
			int num,
			float *values, TIME * times, struct descriptor_xd *limits, int sort);

int paragon_hist___store(struct descriptor *niddsc __attribute__ ((unused)), InStoreStruct * setup)
{
  return Store(niddsc, setup, 0);
}

int paragon_hist___insert(struct descriptor *niddsc __attribute__ ((unused)), InInsertStruct * setup)
{
  return Store(niddsc, (InStoreStruct *) setup, 1);
}

static int Store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup, int sort)
{
  static struct descriptor_d rpt_name = { 0, DTYPE_T, CLASS_D, 0 };
  static struct descriptor_d name = { 0, DTYPE_T, CLASS_D, 0 };
  static struct descriptor_d names[10] = { {0, DTYPE_T, CLASS_D, 0},
  {0, DTYPE_T, CLASS_D, 0},
  {0, DTYPE_T, CLASS_D, 0},
  {0, DTYPE_T, CLASS_D, 0},
  {0, DTYPE_T, CLASS_D, 0},
  {0, DTYPE_T, CLASS_D, 0},
  {0, DTYPE_T, CLASS_D, 0},
  {0, DTYPE_T, CLASS_D, 0},
  {0, DTYPE_T, CLASS_D, 0},
  {0, DTYPE_T, CLASS_D, 0}
  };
  static struct descriptor_xd limits[10] = { {0, DTYPE_DSC, CLASS_XD, 0, 0},
  {0, DTYPE_DSC, CLASS_XD, 0, 0},
  {0, DTYPE_DSC, CLASS_XD, 0, 0},
  {0, DTYPE_DSC, CLASS_XD, 0, 0},
  {0, DTYPE_DSC, CLASS_XD, 0, 0},
  {0, DTYPE_DSC, CLASS_XD, 0, 0},
  {0, DTYPE_DSC, CLASS_XD, 0, 0},
  {0, DTYPE_DSC, CLASS_XD, 0, 0},
  {0, DTYPE_DSC, CLASS_XD, 0, 0},
  {0, DTYPE_DSC, CLASS_XD, 0, 0}
  };

  TIME *tims[10];
  float *vals[10];
  int nums[10];
  int sizes[10];

  int delete_file_nid = setup->head_nid + PARAGON_HIST_N_DELETE_FILE;
  int i;
  int status;
  char line[256];
  FILE *file;
  int isftp;
  status = PARAGON_FTP_COPY(setup->report_name, (struct descriptor *)&rpt_name, &isftp);
  if (!(status & 1)) {
    return (status);
  }
  for (i = 0; i < 10; i++) {
    int name_nid = setup->head_nid + PARAGON_HIST_N_NAME_0 + i;
    nums[i] = 0;
    tims[i] = NULL;
    vals[i] = NULL;
    status = DevText(&name_nid, &names[i]);
    if (status) {
      static int limit_nid;
      static DESCRIPTOR_NID(nid_dsc, &limit_nid);
      limit_nid = setup->head_nid + PARAGON_HIST_N_LIMIT_0 + i;
      status = TdiData((struct descriptor *)&nid_dsc, &limits[i] MDS_END_ARG);
      if (!(status & 1))
	MdsFree1Dx(&limits[i], 0);
      nums[i] = 0;
      sizes[i] = INITIAL_SIZE;
      tims[i] = (TIME *) malloc(sizeof(TIME) * INITIAL_SIZE);
      vals[i] = (float *)malloc(sizeof(float) * INITIAL_SIZE);
    }
  }
  file = fopen(rpt_name.pointer, "r");
  if (file != NULL) {
    while (fgets(line, 255, file)) {
      TIME time;
      float value;
      if (strlen(line) >= 70) {
	if (ParseHistorian(line, (struct descriptor *)&name, &value, &time)) {
	  for (i = 0; i < 10;) {
	    if (StrCompare((struct descriptor *)&name, (struct descriptor *)&names[i]) == 0)
	      break;
	    else
	      i++;
	  }
	  if (i < 10) {
	    if (nums[i] >= sizes[i]) {
	      TIME *t_time = tims[i];
	      float *t_val = vals[i];
	      int j;
	      tims[i] = (TIME *) malloc(sizeof(TIME) * sizes[i] * 2);
	      vals[i] = (float *)malloc(sizeof(float) * sizes[i] * 2);
	      for (j = 0; j < sizes[i]; j++) {
		(tims[i])[j] = t_time[j];
		(vals[i])[j] = t_val[j];
	      }
	      sizes[i] *= 2;
	      free(t_time);
	      free(t_val);
	    }
	    (tims[i])[nums[i]] = time;
	    (vals[i])[nums[i]] = value;
	    nums[i]++;
	  }
	}
      }
    }
    fclose(file);
    for (i = 0; i < 10; i++) {
      if (nums[i]) {
	int value_nid = setup->head_nid + PARAGON_HIST_N_VALUE_0 + i;
	if (TreeIsOn(value_nid) & 1)
	  StoreSignal(value_nid, nums[i], vals[i], tims[i], &limits[i], sort);
      }
      if (vals[i])
	free(vals[i]);
      if (tims[i])
	free(tims[i]);
    }
    PARAGON_FTP_DELETE(setup->report_name, TreeIsOn(delete_file_nid));
  }
  return 1;
}

static void StoreSignal(int nid,
			int num, float *v, TIME * t, struct descriptor_xd *limits, int sort)
{
  DESCRIPTOR_A(vals, sizeof(float), DTYPE_NATIVE_FLOAT, 0, 0);
  DESCRIPTOR_A(tims, sizeof(TIME), DTYPE_QU, 0, 0);
  int status;
  static struct descriptor_xd tmp_xd = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  static struct descriptor_xd values = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  static struct descriptor_xd times = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
  static DESCRIPTOR_SIGNAL_1(signal, &values, 0, &times);
  vals.pointer = (char *)v;
  vals.arsize = num * sizeof(float);
  tims.pointer = (char *)t;
  tims.arsize = num * sizeof(TIME);
  if (limits->l_length) {
    static struct descriptor_xd selections = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
    static DESCRIPTOR(select, "(($1 > $2[0]) && ($1 < $2[1]))");
    static DESCRIPTOR(pack, "PACK($, $)");
    TdiExecute((struct descriptor *)&select, &vals, limits, &selections MDS_END_ARG);
    TdiExecute((struct descriptor *)&pack, &vals, &selections, &values MDS_END_ARG);
    TdiExecute((struct descriptor *)&pack, &tims, &selections, &times MDS_END_ARG);
    MdsFree1Dx(&selections, 0);
  } else {
    MdsCopyDxXd((struct descriptor *)&tims, &times);
    MdsCopyDxXd((struct descriptor *)&vals, &values);
  }
  status = TreeGetRecord(nid, &tmp_xd);
  if (status & 1) {
    static struct descriptor_xd value = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
    static struct descriptor_xd time = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
    //static DESCRIPTOR(sorted_times, "SORT($)");
    static DESCRIPTOR(dim_of, "DIM_OF($)");
    static DESCRIPTOR(val_of, "DATA($)");
    static DESCRIPTOR(set_range, "SET_RANGE(SIZE($1), $1)");
    TdiExecute((struct descriptor *)&dim_of, &tmp_xd, &time MDS_END_ARG);
    TdiExecute((struct descriptor *)&val_of, &tmp_xd, &value MDS_END_ARG);
    TdiVector((struct descriptor *)&value, &values, &values MDS_END_ARG);
    TdiVector((struct descriptor *)&time, &times, &times MDS_END_ARG);
    TdiExecute((struct descriptor *)&set_range, &values, &values MDS_END_ARG);
    TdiExecute((struct descriptor *)&set_range, &times, &times MDS_END_ARG);
    if (sort) {
      static struct descriptor_xd index = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
      static struct descriptor_xd sorted_values = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
      static struct descriptor_xd sorted_times = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
      static struct descriptor_xd sorted_signal = { 0, DTYPE_DSC, CLASS_XD, 0, 0 };
      static DESCRIPTOR_SIGNAL_1(signal, &sorted_values, 0, &sorted_times);
      TdiSort((struct descriptor *)times.pointer, &index MDS_END_ARG);
      TdiSubscript((struct descriptor *)times.pointer, index.pointer, &sorted_times MDS_END_ARG);
      TdiSubscript((struct descriptor *)values.pointer, index.pointer, &sorted_values MDS_END_ARG);
      TdiUnion((struct descriptor *)sorted_times.pointer, &time MDS_END_ARG);
      TdiSubscript((struct descriptor *)&signal, time.pointer, &sorted_signal MDS_END_ARG);
      status = TreePutRecord(nid, (struct descriptor *)&sorted_signal, 0);
      MdsFree1Dx(&index, 0);
      MdsFree1Dx(&sorted_values, 0);
      MdsFree1Dx(&sorted_times, 0);
    } else
      status = TreePutRecord(nid, (struct descriptor *)&signal, 0);
    MdsFree1Dx(&value, 0);
    MdsFree1Dx(&time, 0);
  } else {
    signal.data = (struct descriptor *)&vals;
    signal.dimensions[0] = (struct descriptor *)&tims;
    status = TreePutRecord(nid, (struct descriptor *)&signal, 0);
  }
  MdsFree1Dx(&tmp_xd, 0);
}

static int ParseHistorian(char *line, struct descriptor *name, float *value, TIME * time)
{
  char *format = "\"%d/%d/%d\",\"%d:%d:%d.%d\",\"%22c\",\"%5c\",\"%f \"\n";
  int year, month, day, hrs, mins, secs, frac;

  static int skip_next_value = 0;
  int status = 0;
  char what[6];
  static char name_c[24];
  static struct descriptor name_dsc = { sizeof(name_c), DTYPE_T, CLASS_S, name_c };
  static float fval;
  //static struct descriptor fval_dsc = { sizeof(float), DTYPE_NATIVE_FLOAT, CLASS_S, (char *)&fval };

  if (sscanf(line, format, &month, &day, &year, &hrs, &mins, &secs, &frac, name_c, what, &fval) ==
      10) {
    if (strncmp(what, "START", 5) == 0)
      skip_next_value = 1;
    else {
      if ((!skip_next_value && strncmp(what, "VALUE", 5) == 0) || strncmp(what, "END", 3) == 0) {
	struct tm time_v;
	int tim;
	time_v.tm_year = ((year > 80) ? year + 1900 : year + 2000) - 1900;
	time_v.tm_mon = month;
	time_v.tm_mday = day;
	time_v.tm_hour = hrs;
	time_v.tm_min = mins;
	time_v.tm_sec = secs;
	tim = mktime(&time_v);
	if (tim > 0) {
	  int64_t addin = LONG_LONG_CONSTANT(0x7c95674beb4000);
	  int64_t qtime = ((int64_t) tim) * 10000000 + addin;
	  memcpy((void *)time, &qtime, sizeof(qtime));
	}
	name_dsc.length = strlen(name_c);
	*value = fval;
	StrCopyDx(name, &name_dsc);
	status = 1;
      }
      skip_next_value = 0;
    }
  } else
    skip_next_value = 0;
  return status;
}

EXPORT int PARAGON_FTP_COPY(char *report_in, struct descriptor *report_out, int *isftp)
{
  struct descriptor report = { 0, DTYPE_T, CLASS_S, 0 };
  DESCRIPTOR(ftp_it, "PARAGON_FTP_COPY($)");
  report.length = strlen(report_in);
  report.pointer = report_in;
  *isftp = 1;
  return TdiExecute((struct descriptor *)&ftp_it, &report, report_out MDS_END_ARG);
}

EXPORT int PARAGON_FTP_DELETE(char *report_in, int delete)
{
  int status;
  struct descriptor report = { 0, DTYPE_T, CLASS_S, 0 };
  DESCRIPTOR(ftp_it, "PARAGON_FTP_DELETE($,$)");
  DESCRIPTOR_LONG(delete_d, 0);
  EMPTYXD(ans);
  report.length = strlen(report_in);
  report.pointer = report_in;
  delete_d.pointer = (char *)&delete;
  status = TdiExecute((struct descriptor *)&ftp_it, &report, &delete_d, &ans MDS_END_ARG);
  MdsFree1Dx(&ans, 0);
  return status;
}
