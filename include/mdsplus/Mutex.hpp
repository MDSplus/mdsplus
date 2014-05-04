/*
 * Mutex.hpp
 *
 *  Created on: May 3, 2014
 *      Author: kerickso
 */

#ifndef MDS_MUTEX_HPP_
#define MDS_MUTEX_HPP_

#include <mdsplus/mdsplus.h>
#if defined (MDS_WINDOWS)
#include <windows.h>
#elif defined (MDS_PTHREAD)
#include <pthread.h>
#endif

namespace MDSplus  {

class Mutex {
public:
	Mutex() {
		_create();
	}

	~Mutex() {
		unlock();
		_destroy();
	}

	void lock() { _lock(); }

	void unlock() { _unlock(); }

private:
#if defined (MDS_PTHREAD)
	typedef pthread_mutex_t Mutex_t;
	void _create() { pthread_mutex_init(&mutex, NULL); }
	void _lock() { pthread_mutex_lock(&mutex); }
	void _unlock() { pthread_mutex_unlock(&mutex); }
	void _destroy() { pthread_mutex_destroy(&mutex); }
#elif defined (MDS_WINDOWS)
	typedef HANDLE Mutex_t;
	void _create() { mutex = CreateMutex(NULL, FALSE, NULL);
	void _lock() { WaitForSingleObject(mutex, INFINITE); }
	void _unlock() { ReleaseMutex(mutex); }
	void _destroy() { CloseHandle(mutex); }
#endif

	Mutex_t mutex;
};

class AutoLock {
public:
	AutoLock(Mutex & mutex): mutex(mutex) {
		mutex.lock();
	}

	~AutoLock() {
		mutex.unlock();
	}

private:
	Mutex & mutex;

};

}

#endif /* MDS_MUTEX_HPP_ */
