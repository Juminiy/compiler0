#include "../../include/_alib.hpp"
#include "../../include/_rv64i.hpp"

__DEF_NS__(Alan::RV64::I)

// t0, t1, ..., t6, a0, a1, ..., a7, t0...
std::string GetTempRegister(std::string __reg_, bool __default_ret_ = true)
{
    if(__default_ret_)
        return "a0";
    assert(__reg_.size() == 2);
    char __kind_ = __reg_[0];
    char __cnt_ = __reg_[1];
    switch (__kind_)
    {
    case 't':
        if(__cnt_ >= '0' && __cnt_ <= '5')
            __reg_[1] = __cnt_ + 1;
        else if(__cnt_ == '6')
            __reg_ = "a0";
        else 
            assert(0);
        break;
    case 'a':
        if(__cnt_ >= '0' && __cnt_ <= '6')
            __reg_[1] = __cnt_ + 1;
        else if (__cnt_ == '7')
            __reg_ = "t0";
        else 
            assert(0);
        break;
    default:
        assert(0);
    }
    return __reg_;
}

std::string GetRegisterOf(std::string __val_)
{
    if(__val_ == "0")
        return "x0";
    return "__rand_reg__?";
}


__END_NS__