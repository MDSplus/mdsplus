#include <globus_common.h>
#include <stdio.h>

static globus_mutex_t globus_l_mutex;
static globus_cond_t  globus_l_cond;
static int initialized = 0;

void mdsip_io_wait()
{
  globus_result_t res;
  if (!initialized)
  {
    globus_mutex_init(&globus_l_mutex,NULL);
    globus_cond_init(&globus_l_cond,NULL);
    initialized = 1;
  }
  res = globus_mutex_lock(&globus_l_mutex);
  res = globus_cond_wait(&globus_l_cond,&globus_l_mutex);
  if (res != 0)
    fprintf(stderr,"globus_cond_wait returned %d\n",res);
  res = globus_mutex_unlock(&globus_l_mutex);
}
