#ifndef _JOB_H
#define _JOB_H
#include <pthread_port.h>

#include <mdsmsg.h>

typedef struct job
{
  struct job *next;
  int jobid;
  int conid;
  int *retstatus;
  pthread_rwlock_t *lock;
  void (*callback_done)();
  void (*callback_before)();
  void *callback_param;
  pthread_cond_t *cond;
  int cond_var;
} Job;
#define JOB_PRI "Job(jobid=%d, conid=%d, cond=%d)"
#define JOB_VAR(j) (j)->jobid, (j)->conid, (j)->cond ? (j)->cond_var : -1
pthread_mutex_t jobs_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_JOBS MUTEX_LOCK_PUSH(&jobs_mutex)
#define UNLOCK_JOBS MUTEX_LOCK_POP(&jobs_mutex)
#define UNLOCK_JOBS_REV              \
  pthread_mutex_unlock(&jobs_mutex); \
  pthread_cleanup_push((void *)pthread_mutex_lock, &jobs_mutex)
#define LOCK_JOBS_REV pthread_cleanup_pop(1)
static Job *Jobs = NULL;
static int MonJob = -1;

static Job *newJob(int conid, int *retstatus, pthread_rwlock_t *lock,
                   void (*callback_done)(void *),
                   void *callback_param,
                   void (*callback_before)(void *))
{
  Job *job = (Job *)calloc(1, sizeof(Job));
  job->retstatus = retstatus;
  job->lock = lock;
  job->callback_param = callback_param;
  job->callback_done = callback_done;
  job->callback_before = callback_before;
  job->conid = conid;
  return job;
}

static void Job_pop_locked(Job *job)
{
  Job *j;
  Job **p = &Jobs;
  for (j = Jobs; j; p = &j->next, j = j->next)
  {
    if (j == job)
    {
      MDSDBG(JOB_PRI, JOB_VAR(job));
      *p = j->next;
      free(j);
      break;
    }
  }
}

/// only call this when cond is NULL
static void Job_pop(Job *job)
{
  LOCK_JOBS;
  Job_pop_locked(job);
  UNLOCK_JOBS;
}

static int Job_register(int *msgid,
                        int conid, int *retstatus, pthread_rwlock_t *lock,
                        void *callback_param,
                        void (*callback_done)(void *),
                        void (*callback_before)(void *))
{
  Job *j = newJob(conid, retstatus, lock, callback_param, callback_done, callback_before);
  LOCK_JOBS;
  static int JobId = 0;
  j->jobid = JobId++;
  if (msgid)
  {
    j->cond = malloc(sizeof(pthread_cond_t));
    pthread_cond_init(j->cond, NULL);
    *msgid = j->jobid;
    MDSDBG(JOB_PRI " sync registered", JOB_VAR(j));
  }
  else
  {
    MDSDBG(JOB_PRI " async registered", JOB_VAR(j));
  }
  j->next = Jobs;
  Jobs = j;
  UNLOCK_JOBS;
  return j->jobid;
}

static void Job_callback_before(Job *job)
{
  MDSDBG(JOB_PRI, JOB_VAR(job));
  void *callback_param;
  void (*callback_before)();
  callback_param = job->callback_param;
  callback_before = job->callback_before;
  if (callback_before)
    callback_before(callback_param);
}

/// returns  true if job was popped
static int Job_callback_done(Job *j, int status, int remove)
{
  MDSDBG(JOB_PRI " status=%d, remove=%d", JOB_VAR(j), status, remove);
  void (*callback_done)(void *);
  const int is_mon = j->jobid == MonJob;
  if (j->lock)
    pthread_rwlock_wrlock(j->lock);
  if (j->retstatus)
    *j->retstatus = status;
  callback_done = j->callback_done;
  if (!is_mon)
    j->callback_done = NULL;
  if (j->lock)
    pthread_rwlock_unlock(j->lock);
  if (callback_done)
    callback_done(j->callback_param);
  /**** If job has a condition, RemoveJob will not remove it. ***/
  if (remove && !is_mon)
  {
    int has_cond;
    LOCK_JOBS;
    if ((has_cond = !!j->cond))
    {
      j->cond_var = 1;
      pthread_cond_broadcast(j->cond);
    }
    UNLOCK_JOBS;
    if (!has_cond)
    {
      Job_pop(j);
      return B_TRUE;
    }
  }
  return B_FALSE;
}

/// only call this when cond is NULL

static Job *Job_pop_by_conid(int conid)
{
  Job *j;
  LOCK_JOBS;
  Job **p = &Jobs;
  for (j = Jobs; j; p = &j->next, j = j->next)
  {
    if (j->conid == conid)
    {
      MDSDBG(JOB_PRI, JOB_VAR(j));
      *p = j->next;
      break;
    }
  }
  UNLOCK_JOBS;
  return j;
}

static inline Job *Job_get_by_jobid_locked(int jobid)
{
  Job *j;
  for (j = Jobs; j; j = j->next)
  {
    if (j->jobid == jobid)
    {
      break;
    }
  }
  return j;
}

static inline Job *Job_get_by_jobid(int jobid)
{
  Job *job;
  LOCK_JOBS;
  job = Job_get_by_jobid_locked(jobid);
  UNLOCK_JOBS;
  return job;
}

static void Job_abandon_locked(Job *job)
{
  if (job && job->cond)
  {
    pthread_cond_destroy(job->cond);
    free(job->cond);
    job->cond = NULL;
    MDSDBG(JOB_PRI " sync abandoned!", JOB_VAR(job));
  }
}

static inline void Job_wait_and_pop_locked(Job *job)
{
  pthread_cleanup_push((void *)Job_abandon_locked, (void *)job);
  if (!job->cond_var)
  {
    pthread_cond_wait(job->cond, &jobs_mutex);
  }
  pthread_cond_destroy(job->cond);
  free(job->cond);
  job->cond = NULL;
  Job_pop_locked(job);
  MDSDBG(JOB_PRI " sync done", JOB_VAR(job));
  pthread_cleanup_pop(0);
}

static inline int Job_wait_and_pop_by_jobid(int jobid)
{
  int err;
  LOCK_JOBS;
  Job *job = Job_get_by_jobid_locked(jobid);
  if (job && job->cond)
  {
    MDSDBG(JOB_PRI " sync pending", JOB_VAR(job));
    Job_wait_and_pop_locked(job);
    err = B_FALSE;
  }
  else
  {
    err = B_TRUE;
  }
  UNLOCK_JOBS;
  return err;
}

static Job *Job_pop_by_jobid(int jobid)
{
  Job *j;
  LOCK_JOBS;
  Job **p = &Jobs;
  for (j = Jobs; j; p = &j->next, j = j->next)
  {
    if (j->jobid == jobid)
    {
      MDSDBG(JOB_PRI, JOB_VAR(j));
      *p = j->next;
      break;
    }
  }
  UNLOCK_JOBS;
  return j;
}

static void Job_cleanup(int status, int jobid)
{
  Job *j = Job_pop_by_jobid(jobid);
  if (!j)
    return;
  const int conid = j->conid;
  DisconnectFromMds(conid);
  do
  {
    MDSDBG(JOB_PRI " done", JOB_VAR(j));
    Job_callback_done(j, status, FALSE);
    free(j);
    j = Job_pop_by_conid(conid);
  } while (j);
}
#endif // ifndef _JOB_H