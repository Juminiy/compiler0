#pragma once
#ifndef _REF_HPP
#define _REF_HPP

#include <string>
#include <vector>
#include <map>

class Type 
{
public:
    virtual ~Type() = default;
    virtual std::string TString() const = 0;
};

class Value
{
public:
    virtual ~Value() = default;
    virtual std::string VString() const = 0;
};

class TypeValue: public Type, public Value 
{
public:
    ~TypeValue() = default;
    std::string TString() const {
        return "abstract_type";
    }
    std::string VString() const {
        return "abstract_value";
    }
};

class __Int_ : public TypeValue
{
public:
    int __val_;

    __Int_(int __v) 
        : __val_(__v) { }

    std::string TString() const override{
        return "int";
    }

    std::string VString() const override{
        return std::to_string(__val_);
    }
};

class __Float__ : public TypeValue
{
public:
    float __val_;

    __Float__(float __v)
        : __val_(__v) { }

    std::string TString() const override{
        return "float";
    }

    std::string VString() const override{
        return std::to_string(__val_);
    }
};

class SymTable 
{
public:
    std::map<std::string, TypeValue> __sym_tbl_;

    // if has, return old
    // else, return nil
    TypeValue Put(std::string _sym, TypeValue & _tv) {
        auto _iter = __sym_tbl_.find(_sym);

        if(_iter != __sym_tbl_.end()) {
            __sym_tbl_[_sym] = _tv;
            return _iter->second;
        } else {
            __sym_tbl_.insert(std::make_pair(
                _sym, _tv
            ));
            return TypeValue{};
        }
    }

    // 
    bool Exist(std::string _sym) {
        return __sym_tbl_.find(_sym) == 
                __sym_tbl_.end();
    }

    // 
    TypeValue Get(std::string _sym) {
        auto _iter = __sym_tbl_.find(_sym);
        if (_iter != __sym_tbl_.end()){
            return _iter->second;
        }
        return TypeValue{};
    }

};

#endif // _REF_HPP