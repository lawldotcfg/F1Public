#pragma once

//#include <GSL\\gsl.h>

#define Expects(cond) if(!(cond)) throw(0)

// ugly ugly hack for default construction
// ALWAYS ASIGN TO THIS OR IT IS NOT GARUNTEED TO NOT BE NULL!
template<class T>
class nn_interface
{
    static_assert(std::is_assignable<T&, std::nullptr_t>::value, "T cannot be assigned nullptr.");
public:
	nn_interface(T t) : ptr_(t) { ensure_invariant(); }
	nn_interface& operator=(const T& t) { ptr_ = t; ensure_invariant(); return *this; }

	nn_interface(const nn_interface &other) = default;
	nn_interface& operator=(const nn_interface &other) = default;

    template <typename U, typename Dummy = std::enable_if_t<std::is_convertible<U, T>::value>>
	nn_interface(const nn_interface<U> &other)
    {
        *this = other;
    }

    template <typename U, typename Dummy = std::enable_if_t<std::is_convertible<U, T>::value>>
	nn_interface& operator=(const nn_interface<U> &other)
    {
        ptr_ = other.get();
        return *this;
    }

	// what am i doing
	explicit nn_interface()
    {
	    
    }

    // prevents compilation when someone attempts to assign a nullptr 
	nn_interface(std::nullptr_t) = delete;
	nn_interface(int) = delete;
	nn_interface<T>& operator=(std::nullptr_t) = delete;
	nn_interface<T>& operator=(int) = delete;
    
    T get() const {
#ifdef _MSC_VER
        __assume(ptr_ != nullptr);
#endif
        return ptr_;
    } // the assume() should help the optimizer

    operator T() const {  return get(); }
    T operator->() const { return get(); }

	bool operator==(const T& rhs) const { return ptr_ == rhs; }
	bool operator!=(const T& rhs) const { return !(*this == rhs); }
private:
    T ptr_;

    // we assume that the compiler can hoist/prove away most of the checks inlined from this function
    // if not, we could make them optional via conditional compilation
    void ensure_invariant() const { Expects(ptr_ != nullptr); }

    // unwanted operators...pointers only point to single objects!
    // TODO ensure all arithmetic ops on this type are unavailable
	nn_interface<T>& operator++() = delete;
	nn_interface<T>& operator--() = delete;
	nn_interface<T> operator++(int) = delete;
	nn_interface<T> operator--(int) = delete;
	nn_interface<T>& operator+(size_t) = delete;
	nn_interface<T>& operator+=(size_t) = delete;
	nn_interface<T>& operator-(size_t) = delete;
	nn_interface<T>& operator-=(size_t) = delete;
};