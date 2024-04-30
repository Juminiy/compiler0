#pragma once
#ifndef _PTRLIB_HPP
#define _PTRLIB_HPP

#include <cassert>
#include <memory>
#include "_alib.hpp"

__DEF_NS__(Alan)

// do not permit failure
template < typename _Derived, typename _Base >
std::unique_ptr <_Derived> 
static_uptr_cast
(std::unique_ptr<_Base> &_base) noexcept
{
    auto _derivedPtr = 
        static_cast<_Derived*>(_base.release());

    // program error
    assert(_derivedPtr != nullptr);

    return std::unique_ptr<_Derived>(_derivedPtr);
}

// permit to failure
template < typename _Derived, typename _Base >
std::unique_ptr <_Derived>
dynamic_uptr_cast
(std::unique_ptr<_Base> &_base)
{
    _Base * _basePtr = _base.release();
    _Derived* _derivedPtr = 
        dynamic_cast<_Derived*>(_basePtr);
    if (_derivedPtr)
        return std::unique_ptr<_Derived>(_derivedPtr);
    else 
    {
        _base.reset(_basePtr);
        return nullptr;
    }
}

__END_NS__

#endif