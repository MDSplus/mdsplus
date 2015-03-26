#ifndef MDS_UNITTEST_TESTING_UTILS_H
#define MDS_UNITTEST_TESTING_UTILS_H

#include <stddef.h> // size_t //
#include <vector>

#include <mdsobjects.h> // deleteData //

namespace mds = MDSplus;

namespace testing {

////////////////////////////////////////////////////////////////////////////////
//  Singleton  /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

///
/// Single instance within the same linked module
///
template < typename T >
class Singleton {
public:

    Singleton() {}
    Singleton(const T &copy) {
        this->get_instance() = copy;
    }

    inline T * const operator -> () { return &get_instance(); }
    inline const T * const operator -> () const { return &get_instance(); }

    static T & get_instance() {
        static T instance;
        return instance;
    }

    static const T & get_const_instance() {
        return const_cast<const T&>(get_instance());
    }

private:
//    Singleton(Singleton const&);      // Don't Implement
    void operator=(Singleton const&); // Don't implement
};


////////////////////////////////////////////////////////////////////////////////
//  Comma Init  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Comma Initializer template ...
// ContentT should provide operator[] and resize() methods.
// TODO: Waiting for Static interface mpl check

template < typename ContainerT, typename ContentT >
struct CommaInitializer
{
    typedef ContentT&(ContainerT::* OpType)(const size_t);

    inline explicit CommaInitializer(ContainerT *container,
                                     ContentT s,
                                     OpType op = &ContainerT::operator() )
        : container(container),
          operation(op)
    {
        this->index = 0;
        container->resize(1);
        (container->*operation)(0) = s;
    }

    inline CommaInitializer & operator, (ContentT s) {
        this->index++;
        container->resize(index + 1);
        (container->*operation)(this->index) = s;
        return *this;
    }

    ContainerT *container;
    OpType      operation;
    unsigned int index;
};

////////////////////////////////////////////////////////////////////////////////
//  Unique Ptr  ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

template < typename T >
class Deleter {
public:
    static void _delete(mds::Data * ptr) {
        mds::deleteData(ptr);
    }

    static void _delete(void * ptr) {
        delete (T*)(ptr);
    }
};

template < typename T, typename D = Deleter<T> >
class unique_ptr {
    unique_ptr(const T &ref) : ptr(new T(ref)) {}
    T *ptr;
public:

    unique_ptr() : ptr(NULL) {}

    unique_ptr(unique_ptr &other) : ptr(other.ptr)
    { other.ptr = NULL; }

    unique_ptr(const unique_ptr &other) : ptr(other.ptr)
    { const_cast<unique_ptr&>(other).ptr = NULL; }

    unique_ptr(T *ref) : ptr(ref) { }

    ~unique_ptr() { _delete(); }

    unique_ptr & operator = (T * ref) {
        _delete();
        ptr = ref;
    }

    unique_ptr & operator = (unique_ptr other) {
        ptr = other.ptr;
        other.ptr = NULL;
    }

    void _delete() { if(ptr) D::_delete(ptr); ptr=NULL; }

    operator bool() { return ptr; }

    operator T *() { return ptr; }
    operator const T *() const { return ptr; }

    T * operator ->() { return ptr; }
    const T * operator ->() const { return ptr; }

    T * base() { return ptr; }
    const T * base() const { return ptr; }
};




} // testing


////////////////////////////////////////////////////////////////////////////////
//  COMMA INIT FOR STD CONTAINERS  /////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

namespace std {
template < typename _T >
inline testing::CommaInitializer< std::vector<_T>, _T > operator << (std::vector<_T> &cnt, _T scalar ) {
    return testing::CommaInitializer< std::vector<_T>, _T>(&cnt,scalar,&std::vector<_T>::operator []);
}
} // std


////////////////////////////////////////////////////////////////////////////////
//  Vector Utils ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


namespace std {
template < typename T >
inline ostream &
operator << (ostream &o, const vector<T> &v) {
    typedef typename vector<T>::const_iterator iterator;
    for(iterator it = v.begin(); it < v.end(); ++it) {
        o << *it << " ";
    }
    return o;
}

template < typename T >
inline istream &
operator >> (istream &is, vector<T> &v) {
    T value;
    v.clear();
    while( !(is >> value).fail() )
        v.push_back(value);
    return is;
}


// std implementation only for the same type //
//template<typename _Tp, typename _Alloc>
//  inline bool
//  operator==(const vector<_Tp, _Alloc>& __x, const vector<_Tp, _Alloc>& __y)
//  { return (__x.size() == __y.size()
//        && std::equal(__x.begin(), __x.end(), __y.begin())); }

template < typename _T1, typename _T2, typename _Alloc1, typename _Alloc2 >
inline bool
operator == (const std::vector<_T1,_Alloc1> &v1, const std::vector<_T2,_Alloc2> &v2) {
    if( v1.size() != v2.size() )
        return false;
    for(size_t i=0; i<v1.size(); ++i)
        if( !(v1[i]==static_cast<_T1>(v2[i])) ) return false;
    return true;
}
} // std






#endif // MDS_UNITTEST_TESTING_UTILS_H

