#ifndef MDSTESTUTILS_COMMAINIT_H
#define MDSTESTUTILS_COMMAINIT_H

#include <vector>

////////////////////////////////////////////////////////////////////////////////
//  Comma Init  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Comma Initializer template ...
// ContentT should provide operator[] and resize() methods.

namespace testing
{

  template <typename ContainerT, typename ContentT>
  struct CommaInitializer
  {
    typedef ContentT &(ContainerT::*OpType)(const size_t);

    inline explicit CommaInitializer(ContainerT *container, ContentT s,
                                     OpType op = &ContainerT::operator())
        : container(container), operation(op)
    {
      this->index = 0;
      container->resize(1);
      (container->*operation)(0) = s;
    }

    inline CommaInitializer &operator,(ContentT s)
    {
      this->index++;
      container->resize(index + 1);
      (container->*operation)(this->index) = s;
      return *this;
    }

    ContainerT *container;
    OpType operation;
    unsigned int index;
  };

} // namespace testing

////////////////////////////////////////////////////////////////////////////////
//  COMMA INIT FOR STD CONTAINERS  /////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace std
{
  template <typename _T>
  inline testing::CommaInitializer<std::vector<_T>, _T>
  operator<<(std::vector<_T> &cnt, _T scalar)
  {
    return testing::CommaInitializer<std::vector<_T>, _T>(
        &cnt, scalar, &std::vector<_T>::operator[]);
  }
} // namespace std

#endif // COMMAINIT_H
