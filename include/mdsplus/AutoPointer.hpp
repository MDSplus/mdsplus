/*
 * AutoPointer.hpp
 *
 *  Created on: May 5, 2014
 *      Author: kerickso
 */

#ifndef MDS_AUTOPOINTER_HPP_
#define MDS_AUTOPOINTER_HPP_

#include <vector>
#include <string>

namespace MDSplus {

template<class T>
struct AutoPointer {
	AutoPointer(T * t): ptr(t) {
	}

	~AutoPointer() {
		delete ptr;
	}

	T * ptr;
};

template<class T> struct AutoVector {
	AutoVector(T * vBeg, T* vEnd): vPtr(vBeg), vector(vBeg, vEnd) {
	}

	~AutoVector() {
		delete[] vPtr;
	}

	T * vPtr;
	std::vector<T> vector;
};

struct AutoString {
	AutoString(char * cstr): strPtr(cstr), string(cstr) {
	}

	~AutoString() {
		delete[] strPtr;
	}

	char * strPtr;
	std::string string;
};

}

#endif /* MDS_AUTOPOINTER_HPP_ */
