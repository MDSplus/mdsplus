#ifndef MDSTESTUTILS_FUNCTIONTYPES_H
#define MDSTESTUTILS_FUNCTIONTYPES_H

namespace testing
{

  namespace detail
  {

    ////////////////////////////////////////////////////////////////////////////////
    // type synthesize ( please read: boost implementation  )                     //
    ////////////////////////////////////////////////////////////////////////////////

    template <typename FuncT>
    struct FunctionTypes
    {
    };

    template <typename R, class O>
    struct FunctionTypes<R (O::*)()>
    {
      typedef R ref();
      typedef R(ptr)();
      typedef O obj;
      typedef R ReturnType;
    };

    template <typename R, class O, typename T0>
    struct FunctionTypes<R (O::*)(T0)>
    {
      typedef R ref(T0);
      typedef R(ptr)(T0);
      typedef O obj;
      typedef R ReturnType;
    };

    template <typename R, class O, typename T0, typename T1>
    struct FunctionTypes<R (O::*)(T0, T1)>
    {
      typedef R ref(T0, T1);
      typedef R(ptr)(T0, T1);
      typedef O obj;
      typedef R ReturnType;
    };

    template <typename R, class O, typename T0, typename T1, typename T2>
    struct FunctionTypes<R (O::*)(T0, T1, T2)>
    {
      typedef R ref(T0, T1, T2);
      typedef R(ptr)(T0, T1, T2);
      typedef O obj;
      typedef R ReturnType;
    };

  } // namespace detail
} // namespace testing

#endif // FUNCTIONTYPES_H
