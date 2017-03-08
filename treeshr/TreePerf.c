#include <config.h>
#include <treeshr.h>

#ifdef USE_PERF

#include <treeioperf.h>
#include <sys/mman.h>
#include <treeshr.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

EXPORT int TreeGetPerf(TREE_IO_PERF * perf)
{
  static int initialized = 0;
  static TREE_IO_PERF *p = 0;
  static sem_t *semaphore = 0;
  if (!initialized) {
    initialized = 1;
    char *filename = getenv("mds_perf_filename");
    if (filename != 0) {
      int pf = open(filename, O_RDONLY);
      if (pf != -1) {
	p = mmap(0, sizeof(TREE_IO_PERF), PROT_READ, MAP_SHARED, pf, 0);
	if (p == 0)
	  perror("Error mapping performance file");
	else
	  semaphore = sem_open("mds_perf_lock", O_CREAT, 0664, 1);
      } else
	perror("Error opening performance file");
    }
  }
  if (p != 0 && semaphore != 0) {
    sem_wait(semaphore);
    *perf = *p;
    sem_post(semaphore);
    return TreeNORMAL;
  } else
    return TreeFAILURE;
}

static TREE_IO_PERF *PERF = 0;
static int INITIALIZED = 0;
static sem_t *SEMAPHORE = 0;

static void Initialize()
{
  INITIALIZED = 1;
  char *filename = getenv("mds_perf_filename");
  if (filename != 0) {
    mode_t um = umask(0);
    int pf = open(filename, O_RDWR);
    if (pf == -1) {
      pf = open(filename, O_RDWR | O_CREAT, 0664);
      if (pf != -1) {
	static TREE_IO_PERF init;
	write(pf, &init, sizeof(init));
      }
    }
    if (pf != -1) {
      PERF = mmap(0, sizeof(TREE_IO_PERF), PROT_WRITE, MAP_SHARED, pf, 0);
      if (PERF == 0)
	perror("Error mapping performance file");
      else {
	SEMAPHORE = sem_open("mds_perf_lock", O_CREAT, 0664, 1);
      }
    } else
      perror("Error opening performance file");
    umask(um);
  }
}

EXPORT int TreePerfZero()
{
  if (!INITIALIZED)
    Initialize();
  if (PERF != 0 && SEMAPHORE != 0) {
    sem_wait(SEMAPHORE);
    memset(PERF, 0, sizeof(*PERF));
    sem_post(SEMAPHORE);
    return TreeNORMAL;
  }
  return TreeFAILURE;
}

EXPORT int TreePerfWrite(int bytes)
{
  if (!INITIALIZED)
    Initialize();
  if (PERF != 0 && SEMAPHORE != 0) {
    sem_wait(SEMAPHORE);
    if (bytes < 10) {
      PERF->num_writes_10_0_to_10_1++;
      PERF->write_bytes_10_0_to_10_1 += bytes;
    } else if (bytes < 100) {
      PERF->num_writes_10_1_to_10_2++;
      PERF->write_bytes_10_1_to_10_2 += bytes;
    } else if (bytes < 1000) {
      PERF->num_writes_10_2_to_10_3++;
      PERF->write_bytes_10_2_to_10_3 += bytes;
    } else if (bytes < 100000) {
      PERF->num_writes_10_3_to_10_5++;
      PERF->write_bytes_10_3_to_10_5 += bytes;
    } else if (bytes < 10000000) {
      PERF->num_writes_10_5_to_10_7++;
      PERF->write_bytes_10_5_to_10_7 += bytes;
    } else {
      PERF->num_writes_10_7_to_inf++;
      PERF->write_bytes_10_7_to_inf += bytes;
    }
    sem_post(SEMAPHORE);
    return TreeNORMAL;
  }
  return TreeFAILURE;
}

EXPORT int TreePerfRead(int bytes)
{
  if (!INITIALIZED)
    Initialize();
  if (PERF != 0 && SEMAPHORE != 0) {
    sem_wait(SEMAPHORE);
    if (bytes < 10) {
      PERF->num_reads_10_0_to_10_1++;
      PERF->read_bytes_10_0_to_10_1 += bytes;
    } else if (bytes < 100) {
      PERF->num_reads_10_1_to_10_2++;
      PERF->read_bytes_10_1_to_10_2 += bytes;
    } else if (bytes < 1000) {
      PERF->num_reads_10_2_to_10_3++;
      PERF->read_bytes_10_2_to_10_3 += bytes;
    } else if (bytes < 100000) {
      PERF->num_reads_10_3_to_10_5++;
      PERF->read_bytes_10_3_to_10_5 += bytes;
    } else if (bytes < 10000000) {
      PERF->num_reads_10_5_to_10_7++;
      PERF->read_bytes_10_5_to_10_7 += bytes;
    } else {
      PERF->num_reads_10_7_to_inf++;
      PERF->read_bytes_10_7_to_inf += bytes;
    }
    sem_post(SEMAPHORE);
    return TreeNORMAL;
  }
  return TreeFAILURE;
}
#else
EXPORT int TreeGetPerf()
{
  return TreeFAILURE;
}

EXPORT int TreePerfRead(int bytes __attribute__ ((unused)))
{
  return TreeFAILURE;
}

EXPORT int TreePerfWrite(int bytes __attribute__ ((unused)))
{
  return TreeFAILURE;
}

EXPORT int TreePerfZero()
{
  return TreeFAILURE;
}
#endif
