#include "SystemException.h"
#include "SystemSpecific.h"
#include "Timeout.h"
#include <windows.h>
#define MAX_SEM_COUNT 256
class EXPORT UnnamedSemaphore
{
  //      LPSTR semName;
  char semName[256];

public:
  void initialize(int initVal)
  {

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = FALSE;
    sa.lpSecurityDescriptor = NULL;

    int64_t uniqueId = reinterpret_cast<int64_t>(this);
    while (true)
    {
      sprintf((char *)semName, "%x", uniqueId);
      HANDLE semHandle =
          CreateSemaphore(NULL, initVal, MAX_SEM_COUNT, (LPSTR)semName);
      if (semHandle == 0)
        throw new SystemException("Error initializing semaphore",
                                  GetLastError());
      if (GetLastError() == ERROR_ALREADY_EXISTS)
      {
        CloseHandle(semHandle);
        uniqueId++;
      }
      else
        break;
    }
  }
  void wait()
  {
    HANDLE semHandle =
        OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, (LPSTR)semName);
    if (semHandle == 0)
      throw new SystemException("Error opening semaphore", GetLastError());
    int status = WaitForSingleObject(semHandle, // handle to semaphore
                                     INFINITE); // zero-second time-out interval
    if (status == WAIT_FAILED)
      throw new SystemException("Error waiting Semaphore", GetLastError());
    // CloseHandle(semHandle);
  }

  int timedWait(MdsTimeout &timeout)
  {
    HANDLE semHandle =
        OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, (LPSTR)semName);
    if (semHandle == 0)
      throw new SystemException("Error opening semaphore", GetLastError());
    int status = WaitForSingleObject(
        semHandle,                  // handle to semaphore
        timeout.getTotMilliSecs()); // zero-second time-out interval
    if (status == WAIT_FAILED)
      throw new SystemException("Error waiting Semaphore", GetLastError());
    // CloseHandle(semHandle);
    return status == WAIT_TIMEOUT;
  }

  void post() // return 0 if seccessful
  {
    HANDLE semHandle =
        OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, (LPSTR)semName);
    if (semHandle == 0)
      throw new SystemException("Error opening semaphore", GetLastError());
    if (!ReleaseSemaphore(semHandle, 1, NULL))
      throw new SystemException("Error posting Semaphore", GetLastError());
    // CloseHandle(semHandle);
  }

  bool isZero()
  {
    HANDLE semHandle =
        OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, (LPSTR)semName);
    if (semHandle == 0)
      throw new SystemException("Error opening semaphore", GetLastError());
    int status = WaitForSingleObject(semHandle, // handle to semaphore
                                     0);        // zero-second time-out interval
    if (status == WAIT_FAILED)
      throw new SystemException("Error waiting Semaphore", GetLastError());
    if (status == WAIT_TIMEOUT)
      return true;
    ReleaseSemaphore(semHandle, 1, NULL);
    CloseHandle(semHandle);
    return false;
  }

  void dispose() {}
};
