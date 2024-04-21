#pragma once
#ifndef _RV64I_HPP
#define _RV64I_HPP

#include "_alib.hpp"
#include "_ir.hpp"
#include "koopa.h"

#include <ostream>
#include <vector>

__DEF_NS__(Alan::RV64::I)

class AsmGenerator 
{
public:
    virtual ~AsmGenerator() = default;
    virtual void log(std::ostream &) const = 0;
};

class ValAsmGenerator : public AsmGenerator
{
public:
    std::string GetIns() const noexcept
    {
        switch (__vir_.__kind_)
        {
        case KOOPA_RVT_INTEGER:
            return std::string(
                "li " // move immedidate num
                "a0, " // return number
                ) + 
                std::string(
                    std::find_if(__vir_.__val_->begin(), __vir_.__val_->end(), 
                                ::isdigit),
                    __vir_.__val_->end());

        case KOOPA_RVT_RETURN:
            return "ret";
        default:
            return "";
        }
    }
public:
    explicit ValAsmGenerator
        (Alan::ValueIR & _vir)
        : __vir_(std::move(_vir)){ }

    __DEF_ALL__(vag, ValAsmGenerator);

    __FRIEND_OS_OPT__(vag, _ag)
    {
        __os << "\t" << _ag.GetIns() << __LN__;
        return __os;
    }

    __MEMBER_FUNC_LOG_OVERRIDE__

private:
    Alan::ValueIR __vir_;
};

class BlockAsmGenerator : public AsmGenerator
{
public:
    explicit BlockAsmGenerator
        (Alan::BasicBlockIR &_bbir,
        std::string & _bname)
        // : __bbir_(std::move(_bbir)) 
            :__block_name_(std::move(_bname))
        { 
            for( auto & _vir : _bbir.__instructions_)
                this->__val_asms_.push_back(
                    ValAsmGenerator(*_vir)
                );
        }
    
    __DEF_ALL__(bg, BlockAsmGenerator);

    __FRIEND_OS_OPT__(bg, _ag)
    {   
        for (auto &_vr : _ag.__val_asms_)
        {
            _vr.log(__os);
        }
        return __os;
    }
    
    __MEMBER_FUNC_LOG_OVERRIDE__

private:
    // Alan::BasicBlockIR __bbir_;
public:
    std::vector<ValAsmGenerator> __val_asms_;
    std::string __block_name_;
};

class ProgramAsmGenerator : public AsmGenerator
{
public:
    explicit ProgramAsmGenerator
        (Alan::ProgramIR & _pir)
        // : __pir_(std::move(_pir)) 
        {
            for (auto & UNUSED _gv : _pir.__global_vars_)
            {
                
            }

            for(auto & _fn : _pir.__funcs_) {
                for (auto & _blk : _fn->__basic_blocks_) {
                    this->__block_asms_.push_back(
                        BlockAsmGenerator(*_blk, _fn->__fn_name_)
                    );
                }
            }
            
        }

    __DEF_ALL__(pag, ProgramAsmGenerator);

    __FRIEND_OS_OPT__(pag, _ag)
    {
        // 1. file header
        // example:
        //      .file "hello.s"
        
        // 2. data section
        // example:
        //      .data 
        //          .align 2
        //          .global my_data
        //      my_data:
        //          .word 0xff1122
        __os << "\t.data" << __LN__;
        // to see: how to do
        // for (auto & UNUSED _gv : _ag.__pir_.__global_vars_)
        // {   
        //     __os << "\t.align "; // << align_size << __LN__;
        //     __os << "\t.globl "; // // do some make
        // }

        // 3. instruction sequence
        // example:
        //      .text
        //          .align 2
        //          .global _start
        //      _start:
        //          lui t0, %hi(my_data)
        //          lw t1, %lo(my_data)(t0)
        //          addi t2, t1, 1
        //          sw t2, %lo(my_data)(t0)
        //          jal exit
        __os << "\t.text" << __LN__;
        // block asm generator 
        for (auto & _bs : _ag.__block_asms_)
        {
            __os << "\t.globl " <<_bs.__block_name_ << __LN__;
            __os << _bs.__block_name_ << ":" << __LN__;
            _bs.log(__os);
        }

        __os << "\tret" << __LN__;
        return __os;
    }

    __MEMBER_FUNC_LOG_OVERRIDE__

private:
    // Alan::ProgramIR __ppir_;

public:
    std::vector<BlockAsmGenerator> __block_asms_;

};

__END_NS__

#endif