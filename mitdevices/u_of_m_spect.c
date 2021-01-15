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
#include <string.h>
#include <mdsdescrip.h>
#include <mds_gendevice.h>
#include <mitdevices_msg.h>
#include <mds_stdarg.h>
#include <treeshr.h>
#include <mdsshr.h>
#include <strroutines.h>
#include <libroutines.h>
#include "u_of_m_spect_gen.h"

static int StoreSpectra(int head_nid, int np, int num_frames, int *buffer, int p1, float dt);

#define return_on_error(f,retstatus) if (!((status = f) & 1)) return retstatus;

int u_of_m_spect___init(struct descriptor *niddsc_ptr __attribute__ ((unused)), InInitStruct * setup)
{
  char cmd[512];
  struct descriptor cmd_d={0,DTYPE_T,CLASS_S,cmd};
  int status = 1;
  FILE *file;
  file = fopen(setup->go_file, "w");
  if (file) {
    fprintf(file, "%d\n%d\n%d\n%d\n%d\n%d\n\"Y\"\n", setup->setup_wave_length,
	    setup->setup_slit_width, setup->setup_num_spectra, setup->setup_grating_type,
	    ((setup->setup_exposure + 15) / 30) * 30, setup->setup_filter);
    fclose(file);
    strcpy(cmd, "rm ");
    strcat(cmd, setup->data_file);
    cmd_d.length=strlen(cmd);
    LibSpawn(&cmd_d, 1, 1);
  } else
    status = U_OF_M$_GO_FILE_ERROR;
  return status;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"

int u_of_m_spect___store(struct descriptor *niddsc_ptr __attribute__ ((unused)), InStoreStruct * setup)
{
  int status;
  int frame;
  int i;
  int wavelength, slit, num_frames, grating, exposure, filter, np;
  int total_samps;
  float dt;
  static char comment[132];
  static struct descriptor comment_d = { 0, DTYPE_T, CLASS_S, comment };
  int *buffer;
  FILE *file;
  static int ival;
  static DESCRIPTOR_LONG(ival_d, &ival);
  int comment_nid = setup->head_nid + U_OF_M_SPECT_N_COMMENT;
  int wave_length_nid = setup->head_nid + U_OF_M_SPECT_N_WAVE_LENGTH;
  int slit_width_nid = setup->head_nid + U_OF_M_SPECT_N_SLIT_WIDTH;
  int grating_type_nid = setup->head_nid + U_OF_M_SPECT_N_GRATING_TYPE;
  int exposure_nid = setup->head_nid + U_OF_M_SPECT_N_EXPOSURE;
  int filter_nid = setup->head_nid + U_OF_M_SPECT_N_FILTER;

  file = fopen(setup->data_file, "r");
  if (!file) {
    status = U_OF_M$_DATA_FILE_ERROR;
  } else {
    fscanf(file, "%d %d %d %d %d %d %d\n", &wavelength, &slit, &num_frames, &grating, &exposure,
	   &filter, &np);
    dt = exposure / 1000.;
    fgets(comment, 131, file);
    total_samps = num_frames * np;
    buffer = malloc(total_samps * sizeof(int));
    for (frame = 0; frame < num_frames; frame++) {
      char line[132];
      int rows = np / 10;
      int row;
      int offset = 0;
      for (row = 0; row < rows; row++) {
	offset = frame * np + row * 10;
	fgets(line, 131, file);
	sscanf(line, "%d %d %d %d %d %d %d %d %d %d", &buffer[offset], &buffer[offset + 1],
	       &buffer[offset + 2], &buffer[offset + 3], &buffer[offset + 4], &buffer[offset + 5],
	       &buffer[offset + 6], &buffer[offset + 7], &buffer[offset + 8], &buffer[offset + 9]);
      }
      if (np % 10) {
	for (i = 0; i < (np % 10); i++)
	  fscanf(file, "%d", &buffer[offset + 10 + i]);
	fgets(line, 131, file);
      }
    }
    fclose(file);
    comment_d.length = strlen(comment);
    return_on_error(TreePutRecord(comment_nid, &comment_d, 0), status);
    ival = wavelength;
    return_on_error(TreePutRecord(wave_length_nid, &ival_d, 0), status);
    ival = slit;
    return_on_error(TreePutRecord(slit_width_nid, &ival_d, 0), status);
    ival = grating;
    return_on_error(TreePutRecord(grating_type_nid, &ival_d, 0), status);
    ival = exposure;
    return_on_error(TreePutRecord(exposure_nid, &ival_d, 0), status);
    ival = filter;
    return_on_error(TreePutRecord(filter_nid, &ival_d, 0), status);
    return_on_error(StoreSpectra(setup->head_nid, np, num_frames, buffer, 0, dt), status);
    free(buffer);
  }
  return status;
}
#pragma GCC diagnostic pop

static int StoreSpectra(int head_nid, int np, int num_frames, int *buffer, int p1, float dt)
{
  static int one = 1;
  static DESCRIPTOR_LONG(one_dsc, &one);
  static DESCRIPTOR_LONG(p1_dsc, 0);
  static int end;
  static DESCRIPTOR_LONG(end_dsc, &end);
  static EMPTYXD(first_dim);
  static DESCRIPTOR(range, "$ : $ : $");
  static int zero = 0;
  static DESCRIPTOR_LONG(zero_dsc, &zero);
  static DESCRIPTOR_LONG(num_frames_dsc, 0);
  static int trigger_nid;
  static DESCRIPTOR_NID(trigger_dsc, &trigger_nid);
  static DESCRIPTOR_FLOAT(dt_dsc, 0);
  static EMPTYXD(second_dim);
  static DESCRIPTOR(dim, "BUILD_WITH_UNITS(BUILD_DIM(BUILD_WINDOW($, $, $), * : * : $),'seconds')");
  static EMPTYXD(xd);
  static DESCRIPTOR(sig, "BUILD_SIGNAL($,*,$,$)");
  int status;
  int spectra_nid = head_nid + U_OF_M_SPECT_N_SPECTRA;
  static ARRAY_BOUNDS(int, 2) array = {
    sizeof(int), DTYPE_L, CLASS_A, 0, 0, 0,
    {0, 0, 1, 1, 1}, 2, 0, 0, {0}, {{0,0}}};
  p1_dsc.pointer = (char *)&p1;
  num_frames_dsc.pointer = (char *)&num_frames;
  dt_dsc.pointer = (char *)&dt;
  end = p1 + np;
  trigger_nid = head_nid + U_OF_M_SPECT_N_TRIGGER;
  array.pointer = buffer;
  array.arsize = num_frames * np * sizeof(int);
  array.a0 = buffer - p1;
  array.m[0] = np;
  array.m[1] = num_frames;
  array.bounds[0].l = p1;
  array.bounds[0].u = p1 + np;
  array.bounds[1].l = 0;
  array.bounds[1].u = num_frames - 1;
  return_on_error(TdiCompile((struct descriptor *)&range, &p1_dsc, &end_dsc, &one_dsc, &first_dim MDS_END_ARG), status);
  return_on_error(TdiCompile
		  ((struct descriptor *)&dim, &zero_dsc, &num_frames_dsc, &trigger_dsc, &dt_dsc,
		   &second_dim MDS_END_ARG), status);
  return_on_error(TdiCompile((struct descriptor *)&sig, &array, &first_dim, &second_dim, &xd MDS_END_ARG), status);
  return_on_error(TreePutRecord(spectra_nid, (struct descriptor *)&xd, 0), status);
  MdsFree1Dx(&first_dim, 0);
  MdsFree1Dx(&second_dim, 0);
  MdsFree1Dx(&xd, 0);
  return 1;
}
