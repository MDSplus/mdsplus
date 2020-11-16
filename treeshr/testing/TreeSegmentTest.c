#include <treeshr.h>
#include <mdsdescrip.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 3
#define NUM_SEGS 100
#define SEG_SZE 10000
#define NODEFMTSTR "S%02d"

#define OK(m) ({int r = (m);if ((r & 1) == 0) fprintf(stdout, "%d: %d - %s\n", num, r, #m);})

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int go = 0;
static char const tree[] = "test";
static int const shot = 1;

void *job(void* args)
{
    int num = (int)(intptr_t)args;
    void *DBID = NULL;
    int nid;
    char name[13];
    OK(_TreeOpen(&DBID, tree, shot, 0));
    sprintf(name, NODEFMTSTR, num);
    uint64_t start = 0, end = SEG_SZE - 1, *dim;
    dim = (__typeof__(dim))malloc(SEG_SZE*sizeof(*dim));
    int16_t *data = (__typeof__(data))malloc(SEG_SZE*sizeof(*data));
    mdsdsc_t dstart = { 8, DTYPE_QU, CLASS_S, (char*)&start };
    mdsdsc_t dend = { 8, DTYPE_QU, CLASS_S, (char*)&end };
    DESCRIPTOR_A(ddim, sizeof(*dim), DTYPE_QU, dim, SEG_SZE*sizeof(*dim));
    DESCRIPTOR_A(ddata, sizeof(*data), DTYPE_W, data, SEG_SZE*sizeof(*data));
    for ( int i = 0 ; i < SEG_SZE ; i++ )
        data[i] = i;
    OK(_TreeFindNode(DBID, name, &nid));
    pthread_mutex_lock(&mutex);
    while (go == 0)
        pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
    fprintf(stderr, "%d: nid = %d\n", num, nid);
    for ( int i = 0 ; i < NUM_SEGS ; i++ )
    {
        for ( int j = 0 ; j < SEG_SZE ; j++ )
            dim[j] = i * SEG_SZE + j;
        start = dim[0];
        end = dim[SEG_SZE-1];
        //pthread_mutex_lock(&mutex);
        OK(_TreeMakeSegment(DBID, nid, &dstart, &dend, (mdsdsc_t*)&ddim, (mdsdsc_a_t*)&ddata, -1, SEG_SZE));
        //pthread_mutex_unlock(&mutex);
    }
    OK(_TreeClose(&DBID, NULL, 0));
    return NULL;
}


int main(int const arc, char const *const argv[])
{
    int num = -1;
    void *DBID = NULL;
    OK(_TreeOpenNew(&DBID, tree, shot));
    int nid;
    char name[13];
    for ( int i = 0 ; i < NUM_THREADS ; i++ )
    {
        sprintf(name, NODEFMTSTR, i);
        OK(_TreeAddNode(DBID, name, &nid, 6));
    }
    OK(_TreeWriteTree(&DBID, NULL, 0));
    OK(_TreeCleanDatafile(&DBID, tree, shot)); // includes close
    pthread_t threads[NUM_THREADS];
    for ( int i = 0 ; i < NUM_THREADS ; i++ )
        pthread_create(&threads[i], NULL, job, (void*)(intptr_t)i);
    pthread_mutex_lock(&mutex);
    go = 1;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    for ( int i = 0 ; i < NUM_THREADS ; i++ )
        pthread_join(threads[i], NULL);
    OK(_TreeOpen(&DBID, tree, shot, 1));
    mdsdsc_xd_t xd = {0, DTYPE_DSC, CLASS_XD, NULL, 0};
    for ( int num = 0 ; num < NUM_THREADS ; num++ )
    {
        sprintf(name, NODEFMTSTR, num);
        OK(_TreeFindNode(DBID, name, &nid));
        OK(_TreeGetRecord(DBID, nid, &xd));
    }
    OK(_TreeClose(&DBID, NULL, 0));
}