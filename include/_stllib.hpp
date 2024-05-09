#pragma once
#ifndef _STLLIB_HPP
#define _STLLIB_HPP

#include <cassert>
#include <memory>

#include <deque>
#include <stack>
#include <algorithm>
#include <iterator>

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

template < typename _Tp >
__GEN_FUNC_COPY__
void __debug_stack_(std::stack<_Tp> __stk_)
    noexcept
{
    auto sdeq = std::deque<_Tp>();
    PRINT("[ ");
    // switch (__stk_.size())
    // {
    // case 0:
    //     break;
    // default:
    //     PRINT(__stk_.top());
    //     __stk_.pop();
    //     break;
    // }
    // while(!__stk_.empty())
    // {
    //     PRINT(", "), PRINT(__stk_.top());
    //     __stk_.pop();
    // }
    while(!__stk_.empty())
    {
        sdeq.push_back(__stk_.top());
        __stk_.pop();
    }
    while(!sdeq.empty())
    {   
        PRINT(", "), PRINT(sdeq.back());
        sdeq.pop_back();
    }

    PRINTLN(" ]");
}   

template < typename _Container >
void LogContainer(const _Container &__container,
                        const char* __delimiter = ", ")
{
    PRINT("[");

    size_t _csz = __container.size();
    if (_csz == 1)
    {
        PRINT(*__container.cbegin());
    } else if (_csz > 1)
    {
        auto _it  = __container.begin();
        auto _eit = __container.begin();
        std::advance(_eit, _csz-1);

        std::copy(_it, _eit,
                std::ostream_iterator
                < typename _Container::value_type >
                    (std::cout, __delimiter));
        PRINT(*_eit);
    }

    PRINTLN("]");
}

#define __str2int_(__str_) std::strtol(__str_.c_str(), nullptr, 10)
#define __int2str_(__i32_) std::to_string(__i32_)

__END_NS__

#endif