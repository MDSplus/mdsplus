#ifndef MDSTESTUTILS_SINGLETON_H
#define MDSTESTUTILS_SINGLETON_H

namespace testing
{

  ////////////////////////////////////////////////////////////////////////////////
  //  Singleton  /////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////

  ///
  /// Single instance within the same linked module
  ///
  template <typename T>
  class Singleton
  {
  public:
    Singleton() {}
    Singleton(const T &copy) { this->get_instance() = copy; }

    inline T *operator->() { return &get_instance(); }
    inline const T *operator->() const { return &get_instance(); }

    static T &get_instance()
    {
      static T instance;
      return instance;
    }

    static const T &get_const_instance()
    {
      return const_cast<const T &>(get_instance());
    }

  private:
    //    Singleton(Singleton const&);      // Don't Implement
    void operator=(Singleton const &); // Don't implement
  };

} // namespace testing

#endif // SINGLETON_H
