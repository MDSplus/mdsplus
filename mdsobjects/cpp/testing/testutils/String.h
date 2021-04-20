#ifndef TESTUTILS_STRING_H
#define TESTUTILS_STRING_H

#include <string>
#include <algorithm>

namespace testing
{
  std::string toupper(std::string str)
  {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
  }

} // namespace testing

#endif // STRING_H
