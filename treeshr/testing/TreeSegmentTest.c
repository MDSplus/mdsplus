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
#include <mdsshr.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <treeshr.h>
#include <unistd.h>

static int NUM_THREADS = 3;
static int NUM_SEGS = 100;
static int SEG_SZE = 10000;
#define NODEFMTSTR "S%02d"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int go = 0;
static char const tree[] = "tree_test";
static int const shot = 1;
static int result = 0;
#define TEST_STATUS(m)                              \
  ({                                                \
    int r = (m);                                    \
    if ((r & 1) == 0)                               \
    {                                               \
      fprintf(stdout, "%d: %d - %s\n", num, r, #m); \
      pthread_mutex_lock(&mutex);                   \
      result = 1;                                   \
      pthread_mutex_unlock(&mutex);                 \
    }                                               \
  })

void *job(void *args)
{
  int i, j, num = (int)(intptr_t)args;
  void *DBID = NULL;
  int nid;
  char name[13];
  TEST_STATUS(_TreeOpen(&DBID, tree, shot, 0));
  sprintf(name, NODEFMTSTR, num);
  uint64_t start = 0, end = SEG_SZE - 1, *dim;
  dim = (__typeof__(dim))malloc(SEG_SZE * sizeof(*dim));
  int16_t *data = (__typeof__(data))malloc(SEG_SZE * sizeof(*data));
  mdsdsc_t dstart = {8, DTYPE_QU, CLASS_S, (char *)&start};
  mdsdsc_t dend = {8, DTYPE_QU, CLASS_S, (char *)&end};
  DESCRIPTOR_A(ddim, sizeof(*dim), DTYPE_QU, dim, SEG_SZE * sizeof(*dim));
  DESCRIPTOR_A(ddata, sizeof(*data), DTYPE_W, data, SEG_SZE * sizeof(*data));
  for (i = 0; i < SEG_SZE; i++)
    data[i] = i;
  TEST_STATUS(_TreeFindNode(DBID, name, &nid));
  pthread_mutex_lock(&mutex);
  while (go == 0)
    pthread_cond_wait(&cond, &mutex);
  pthread_mutex_unlock(&mutex);
  fprintf(stderr, "%d: nid = %d\n", num, nid);
  for (i = 0; i < NUM_SEGS; i++)
  {
    for (j = 0; j < SEG_SZE; j++)
      dim[j] = i * SEG_SZE + j;
    start = dim[0];
    end = dim[SEG_SZE - 1];
    TEST_STATUS(_TreeMakeSegment(DBID, nid, &dstart, &dend, (mdsdsc_t *)&ddim,
                                 (mdsdsc_a_t *)&ddata, -1, SEG_SZE));
  }
  free(data);
  free(dim);
  TEST_STATUS(_TreeClose(&DBID, NULL, 0));
  TreeFreeDbid(DBID);
  return NULL;
}

int main(int const argc, char const *const argv[])
{
  int a = 0;
  if (argc > ++a)
    NUM_THREADS = atoi(argv[a]);
  if (argc > ++a)
    NUM_SEGS = atoi(argv[a]);
  if (argc > ++a)
    SEG_SZE = atoi(argv[a]);
  int num = -1;
  TEST_STATUS(MdsPutEnv("tree_test_path=."));
  void *DBID = NULL;
  TEST_STATUS(_TreeOpenNew(&DBID, tree, shot));
  int nid;
  char name[13];
  int i, j;
  for (i = 0; i < NUM_THREADS; i++)
  {
    sprintf(name, NODEFMTSTR, i);
    TEST_STATUS(_TreeAddNode(DBID, name, &nid, 6));
  }
  TEST_STATUS(_TreeWriteTree(&DBID, NULL, 0));
  TEST_STATUS(_TreeCleanDatafile(&DBID, tree, shot)); // includes close
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, 0x40000);
  pthread_t threads[NUM_THREADS];
  for (i = 0; i < NUM_THREADS; i++)
    pthread_create(&threads[i], &attr, job, (void *)(intptr_t)i);
  pthread_attr_destroy(&attr);
  pthread_mutex_lock(&mutex);
  go = 1;
  pthread_cond_broadcast(&cond);
  pthread_mutex_unlock(&mutex);
  for (i = 0; i < NUM_THREADS; i++)
    pthread_join(threads[i], NULL);
  TEST_STATUS(_TreeOpen(&DBID, tree, shot, 1));
  mdsdsc_xd_t xd = {0, DTYPE_DSC, CLASS_XD, NULL, 0};
  for (num = 0; num < NUM_THREADS; num++)
  {
    sprintf(name, NODEFMTSTR, num);
    TEST_STATUS(_TreeFindNode(DBID, name, &nid));
    TEST_STATUS(_TreeGetRecord(DBID, nid, &xd));
    if (xd.l_length == 0 || xd.pointer == NULL ||
        xd.pointer->class != CLASS_R)
    {
      fprintf(stderr, "%d: invalid record", num);
      result = 1;
      continue;
    }
    mdsdsc_r_t const *r = (mdsdsc_r_t const *)xd.pointer;
    if (r->dscptrs[0]->class != CLASS_A || r->dscptrs[0]->dtype != DTYPE_W)
    {
      fprintf(stderr, "%d: invalid data type", num);
      result = 1;
      continue;
    }
    int16_t const *data = (int16_t const *)r->dscptrs[0]->pointer;
    int arr_length = ((mdsdsc_a_t *)r->dscptrs[0])->arsize / 2;
    if (arr_length != SEG_SZE * NUM_SEGS)
    {
      fprintf(stderr, "%d: invalid data length %d of %d", num, arr_length,
              SEG_SZE * NUM_SEGS);
      result = 1;
      continue;
    }
    for (j = 0; j < NUM_SEGS; j++)
    {
      for (i = 0; i < SEG_SZE; i++)
      {
        if (data[i + j * SEG_SZE] != (int16_t)i)
        {
          fprintf(stderr, "%d: invalid data[i] is %d, but %d expected", num,
                  data[i + j * SEG_SZE], i);
          result = 1;
          break;
        }
      }
    }
  }
  TEST_STATUS(MdsFree1Dx(&xd, NULL));
  TEST_STATUS(_TreeClose(&DBID, NULL, 0));
  TreeFreeDbid(DBID);
  return result;
}