#ifndef MDSTESTUTILS_TESTING_H
#define MDSTESTUTILS_TESTING_H

#include <assert.h>
#include <cmath>
#include <limits>
#include <stdio.h>

namespace testing
{

  template <typename T>
  bool areSame(T a, T b)
  {
    return std::fabs(a - b) < std::numeric_limits<T>::epsilon();
  }

} // namespace testing

// dummy linked symbol that is called by tests //
extern "C" int get_ghostbusters_phone();
extern "C" int test_nextint(int *x);
extern "C" int test_addint(int *a, int *b);

#endif // MDSTESTUTILS_TESTING_H
