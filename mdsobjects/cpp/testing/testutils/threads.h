#ifndef TESTUTILS_THREADS_H
#define TESTUTILS_THREADS_H

#include <pthread.h>

////////////////////////////////////////////////////////////////////////////////
// THREAD BASE  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace testing {
class Thread
{
public:
   Thread() {}
   virtual ~Thread() {}

   bool StartThread() {
      return (pthread_create(&_thread, NULL, InternalThreadEntryFunc, this) == 0);
   }

   void StopThread() {
       pthread_cancel(_thread);
   }

   void WaitForThreadToExit() {
      (void) pthread_join(_thread, NULL);
   }

protected:
   virtual void InternalThreadEntry() = 0;

private:
   static void * InternalThreadEntryFunc(void * This) {
       ((Thread *)This)->InternalThreadEntry();
       return NULL;
   }

   pthread_t _thread;
};
} // testing


#endif // THREADS_H

