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
#include "SystemSpecific.h"
#include "SystemException.h"
#include "Timeout.h"
#include <windows.h>
#define MAX_SEM_COUNT 256
class EXPORT UnnamedSemaphore {
//      LPSTR semName;
  char semName[256];
 public:
  void initialize(int initVal) {

    SECURITY_ATTRIBUTES sa;
     sa.nLength = sizeof(SECURITY_ATTRIBUTES);
     sa.bInheritHandle = FALSE;
     sa.lpSecurityDescriptor = NULL;

    int64_t uniqueId = reinterpret_cast < int64_t > (this);
    while (true) {
      sprintf((char *)semName, "%x", uniqueId);
      HANDLE semHandle = CreateSemaphore(NULL, initVal, MAX_SEM_COUNT, (LPSTR) semName);
      if (semHandle == 0)
	throw new SystemException("Error initializing semaphore", GetLastError());
      if (GetLastError() == ERROR_ALREADY_EXISTS) {
	CloseHandle(semHandle);
	uniqueId++;
      } else
	 break;
    }
  }
  void wait() {
    HANDLE semHandle = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, (LPSTR) semName);
    if (semHandle == 0)
      throw new SystemException("Error opening semaphore", GetLastError());
    int status = WaitForSingleObject(semHandle,	// handle to semaphore
				     INFINITE);	// zero-second time-out interval
    if (status == WAIT_FAILED)
      throw new SystemException("Error waiting Semaphore", GetLastError());
    //CloseHandle(semHandle);
  }

  int timedWait(MdsTimeout & timeout) {
    HANDLE semHandle = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, (LPSTR) semName);
    if (semHandle == 0)
      throw new SystemException("Error opening semaphore", GetLastError());
    int status = WaitForSingleObject(semHandle,	// handle to semaphore
				     timeout.getTotMilliSecs());	// zero-second time-out interval
    if (status == WAIT_FAILED)
      throw new SystemException("Error waiting Semaphore", GetLastError());
    //CloseHandle(semHandle);
    return status == WAIT_TIMEOUT;
  }

  void post()			//return 0 if seccessful
  {
    HANDLE semHandle = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, (LPSTR) semName);
    if (semHandle == 0)
      throw new SystemException("Error opening semaphore", GetLastError());
    if (!ReleaseSemaphore(semHandle, 1, NULL))
      throw new SystemException("Error posting Semaphore", GetLastError());
    //CloseHandle(semHandle);
  }

  bool isZero() {
    HANDLE semHandle = OpenSemaphore(SEMAPHORE_ALL_ACCESS, NULL, (LPSTR) semName);
    if (semHandle == 0)
      throw new SystemException("Error opening semaphore", GetLastError());
    int status = WaitForSingleObject(semHandle,	// handle to semaphore
				     0);	// zero-second time-out interval
    if (status == WAIT_FAILED)
      throw new SystemException("Error waiting Semaphore", GetLastError());
    if (status == WAIT_TIMEOUT)
      return true;
    ReleaseSemaphore(semHandle, 1, NULL);
    CloseHandle(semHandle);
    return false;
  }

  void dispose() {
  }
};
