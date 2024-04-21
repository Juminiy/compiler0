#pragma once
#ifndef _ALIB_HPP
#define _ALIB_HPP

// migrate Macro
// Macro namespace
#define __DEF_NS__(__ns__) \
        namespace __ns__ {
#define __END_NS__ }

#define __USE_NS__(__ns__) \
        using namespace __ns__

// Macro GCC extension
#define UNUSED __attribute__((unused))
#define NORETURN __attribute__((noreturn))
#define CONSTRUCT __attribute__((constructor))
#define DESTRUCT __attribute__((destructor))

// Macro evaluate expression
#define __STRFY__(__expr_x__) #__expr_x__
#define __TOSTR__(__expr_x__) __STRFY__(__expr_x__)

// Macro color decleration
#define _COLOR_BLACK    0
#define _COLOR_RED      1
#define _COLOR_GREEN    2
#define _COLOR_YELLOW   3
#define _COLOR_BLUE     4
#define _COLOR_PURPLE   5
#define _COLOR_CYAN     6
#define _COLOR_WHITE    7

#define _COLOR_HEAD "\033[0;3"
#define _COLOR_TAIL "\033[0m"
#define _COLOR_CL(_cl_, _ct_) \
        (_COLOR_HEAD \
        __TOSTR__(_cl_)"m" + \
        std::string(_ct_) + \
        _COLOR_TAIL)
#define _COLOR_CL_VAR(_cl_, _ct_) \
        (_COLOR_HEAD + \
        std::string(_cl_) + \
        "m" + \
        std::string(_ct_) + \
        _COLOR_TAIL)
#define _BLACK(_ct_) \
        _COLOR_CL(_COLOR_BLACK, _ct_)
#define _RED(_ct_) \
        _COLOR_CL(_COLOR_RED, _ct_)
#define _GREEN(_ct_) \
        _COLOR_CL(_COLOR_GREEN, _ct_)
#define _YELLOW(_ct_) \
        _COLOR_CL(_COLOR_YELLOW, _ct_)
#define _BLUE(_ct_) \
        _COLOR_CL(_COLOR_BLUE, _ct_)
#define _PURPLE(_ct_) \
        _COLOR_CL(_COLOR_PURPLE, _ct_)
#define _CYAN(_ct_) \
        _COLOR_CL(_COLOR_CYAN, _ct_)
#define _WHITE(_ct_) \
        _COLOR_CL(_COLOR_WHITE, _ct_)

#define _COLOR_START(_cl_) \
        PRINT(_COLOR_HEAD \
        __TOSTR__(_cl_)"m")
#define _COLOR_END PRINT(_COLOR_TAIL)
#define _COLOR_RECOVER _COLOR_END

// Macro print
#define __LN__ '\n'
#define __FLUSH_LN__ std::endl
#define INPUT(_is_, _ct_) \
        _is_ >> _ct_
#define OUTPUT(_os_, _ct_) \
        _os_ << _ct_
#define OUTPUTLN(_os_, _ct_) \
        OUTPUT(_os_, _ct_) << __LN__
#define OUTPUT_DETAIL(_os_, _ct_) \
        _os_ << #_ct_ << " = " << _ct_
#define OUTPUTLN_DETAIL(_os_, _ct_) \
        OUTPUT_DETAIL(_os_, _ct_) << __LN__
#define __ONEW_LINE__(_os_) \
        _os_ << __LN__
#define NEXT_LINE \
        __CNEW_LINE__

#define HANDINPUT(_ct_) \
        INPUT(std::cin, _ct_)
#define PRINT(_ct_) \
        OUTPUT(std::cout, _ct_)
#define PRINTLN(_ct_) \
        PRINT(_ct_) << __LN__
#define PRINT_DETAIL(_ct_) \
        OUTPUT_DETAIL(std::cout, _ct_)
#define PRINTLN_DETAIL(_ct_) \
        PRINT_DETAIL(_ct_) << __LN__
#define __CNEW_LINE__ \
        __ONEW_LINE__(std::cout)

#define ERR(_ct_) \
        OUTPUT(std::cerr, _ct_)
#define ERRLN(_ct_) \
        ERR(_ct_) << __LN__
#define ERR_DETAIL(_ct_) \
        OUTPUT_DETAIL(std::cerr, _ct_)
#define ERRLN_DETAIL(_ct_) \
        ERR_DETAIL(_ct_) << __LN__

// Macro using(typedef)
#define __CONST__(__type__) \
        __type__ const
#define __REF__(__type__) \
        __type__ &
#define __CONST_REF__(__type__) \
        __type__ const &
#define __RREF__(__type__) \
        __type__ &&
#define __UREF__(__tp__) \
		__tp__ &&
#define __PTR__(__type__) \
        __type__ *
#define __CONST_PTR__(__type__) \
        __type__ * const
#define __PTR_TO_CONST__(__type__) \
        __type__ const *
#define __CONST_PTR_TO_CONST__(__type__) \
        __type__ const * const

#define __DEF_SZX__ \
    using size_type = size_t; \
    using difference_type = ptrdiff_t
// template type _Tp define
#define __DEF_TPL__(__tp__) \
	using value_type = __tp__ ; \
	using const_type = __CONST__(__tp__); \
	using reference = __REF__(__tp__); \
	using const_reference = __CONST_REF__(__tp__); \
	using r_reference = __RREF__(__tp__); \
	using pointer = __PTR__(__tp__); \
	using const_pointer = __CONST_PTR__(__tp__); \
	using pointer_to_const = __PTR_TO_CONST__(__tp__); \
	using const_pointer_to_const = __CONST_PTR_TO_CONST__(__tp__); \
	__DEF_SZX__

#define __DEF_DCL_V2__(__wrap_type__, __meta_type__, __decl__) \
        using __meta_type__ ## __decl__ = __wrap_type__

#define __DEF_CONST_V2__(__type__) \
        __DEF_DCL_V2__(__CONST__(__type__), __type__, _const)
#define __DEF_REF_V2__(__type__) \
        __DEF_DCL_V2__(__REF__(__type__), __type__, _reference)
#define __DEF_CREF_V2__(__type__) \
        __DEF_DCL_V2__(__CONST_REF__(__type__), __type__, _const_reference)
#define __DEF_RREF_V2__(__type__) \
        __DEF_DCL_V2__(__RREF__(__type__), __type__, _r_reference)
#define __DEF_PTR_V2__(__type__) \
        __DEF_DCL_V2__(__PTR__(__type__), __type__, _pointer)
#define __DEF_CONST_PTR_V2__(__type__) \
        __DEF_DCL_V2__(__CONST_PTR__(__type__), __type__, _const_pointer)
#define __DEF_PTR_TO_CONST_V2__(__type__) \
        __DEF_DCL_V2__(__PTR_TO_CONST__(__type__), __type__, _pointer_to_const)
#define __DEF_CONST_PTR_TO_CONST_V2__(__type__) \
        __DEF_DCL_V2__(__CONST_PTR_TO_CONST__(__type__), __type__, _const_pointer_to_const)

// use :	__DEF_DCL_V2__
// detail:	using __alias_type__ = __meta_type__##__xx_xx__;
#define __DEF_ALL__(__alias_type__, __meta_type__) \
        using __alias_type__ = __meta_type__ ; \
        __DEF_CONST_V2__(__alias_type__); \
        __DEF_REF_V2__(__alias_type__); \
        __DEF_CREF_V2__(__alias_type__); \
        __DEF_RREF_V2__(__alias_type__); \
        __DEF_PTR_V2__(__alias_type__); \
        __DEF_CONST_PTR_V2__(__alias_type__); \
        __DEF_PTR_TO_CONST_V2__(__alias_type__); \
        __DEF_CONST_PTR_TO_CONST_V2__(__alias_type__)

#define __FRIEND_OS_OPT__(__tp__, __tp_val_) \
        friend std::ostream& \
            operator << (std::ostream& __os, __CONST_REF__(__tp__) __tp_val_)
#define __MEMBER_FUNC_LOG_OVERRIDE__ \
        void log(std::ostream& __os) const override \
        { __os << *this; }
#define __MEMBER_FUNC_LOG_DECL__ \
	void log(std::ostream& __os) const override
#define __MEMBER_FUNC_LOG_DEF__(__tp__) \
        void __tp__::log(std::ostream& __os) const \
        { __os << *this; }

#define __MV_ASSI__(__alias_class__, __def_full_mv__) \
        void assign(__alias_class__ ##_reference _c) \
        noexcept { __def_full_mv__ } 

#define __MV_SMTC__(__alias_class__, __class__) \
        __class__(__alias_class__ ##_r_reference _mv_c) \
        { this->assign(_mv_c); } \
        __alias_class__ ##_reference operator = \
        (__alias_class__ ##_r_reference _mv_c) \
        { \
            if (this != &_mv_c) [[likely]] \
                this->assign(_mv_c); \
            return *this; \
        }

#define __GEN_FUNC_COPY__ static inline

// Macro parse argv
#define MODE_UNDEF -1
#define MODE_AST 0
#define MODE_IR 1
#define MODE_ASM 2
#define MODE_ALL 3

#include <cstdlib>
#include <cstring>

__GEN_FUNC_COPY__
int parseMode 
(const char * _mode) 
{
    if (!::strcmp(_mode, "-ast"))
        return MODE_AST;
    else if (!::strcmp(_mode, "-koopa"))
        return MODE_IR;
    else if (!::strcmp(_mode, "-asm")||
			!::strcmp(_mode, "-riscv"))
        return MODE_ASM;
    else if(!::strcmp(_mode, "-all"))
        return MODE_ALL;
    return MODE_UNDEF;
};

// 012345678910
// test/hello.s
__GEN_FUNC_COPY__
char * parseGetOutputByInput
(const char * _input, const char * _suffix UNUSED = ".s")
{   
    size_t _i = 0, _st = 0;
    while( _i < ::strlen(_input) && 
            _input[_i] != '.')
    {
        // if (_input[_i] == '/')
        //     _st = _i + 1;
        // else 
        if (_input[_i] == '.')
            break;
        ++_i;
    }
    char * __output = 
        (char *)(malloc(sizeof(char) * (_i-_st)));
    ::strncpy(__output, _input + _st, _i-_st);
    return __output;
}

__GEN_FUNC_COPY__
char * _strcat_dup(const char * _src, const char * _const)
{
    return ::strcat(::strdup(_src), _const);
}

#define __STR_CAT__(__src__, __const__) \
        _strcat_dup(__src__, __const__)

#endif