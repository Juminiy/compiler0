#pragma once
#ifndef _PTRLIB_HPP
#define _PTRLIB_HPP

#include <cassert>
#include <memory>
#include "_alib.hpp"

__DEF_NS__(Alan)

template < typename _Derived, typename _Base >
std::unique_ptr <_Derived> 
dynamic_uptr_cast
(std::unique_ptr<_Base> &_base) noexcept
{
    // auto _rawBasePtr = _base.release();
    _Derived* _derivedPtr = 
        dynamic_cast<_Derived*>(_base.release());

    // program error
    assert(_derivedPtr != nullptr);

    return std::unique_ptr<_Derived>(_derivedPtr);
}

__END_NS__

#endif