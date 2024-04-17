#pragma once
#ifndef _AST_HPP
#define _AST_HPP

#include <memory>
#include <string>
#include <iostream>

// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;
  virtual void log(std::ostream &) const = 0;
};

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST {
 public:
  // 用智能指针管理对象
  std::unique_ptr<BaseAST> func_def;
  
  friend std::ostream &
    operator << (std::ostream & __os, const CompUnitAST & _a)
    {
        __os << "CompUnit { ";
        _a.func_def->log(__os);
        __os << " }\n";
        return __os;
    }
  void log(std::ostream & __os) const override{
    __os << *this;
  }

};

// FuncDef 也是 BaseAST
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::unique_ptr<std::string> ident;
  std::unique_ptr<BaseAST> block;
  friend std::ostream& 
    operator << (std::ostream& __os, const FuncDefAST & _a)
    {
        __os << "FuncDef { ";
        _a.func_type->log(__os);
        __os << ", " << *_a.ident << ", ";
        _a.block->log(__os);
        __os << " }";
        return __os;
    }
    void log(std::ostream & __os) const override{
        __os << *this;
    }
};

class FuncTypeAST : public BaseAST {
public:
    std::unique_ptr<std::string> __type_;

    friend std::ostream& 
        operator << (std::ostream & __os, const FuncTypeAST &_a)
        {
            __os << *_a.__type_;
            return __os;
        }
    void log(std::ostream & __os) const override{
        __os << *this;
    }
};

class BlockAST : public BaseAST {
public:
    std::unique_ptr<BaseAST> __stmt_;

    friend std::ostream& 
        operator << (std::ostream & __os, const BlockAST &_a)
        {
            __os << "Block { ";
            _a.__stmt_->log(__os);
            __os << " }";
            return __os;
        }
    void log(std::ostream & __os) const override{
        __os << *this;
    }
};

class StmtAST : public BaseAST {
public:
    std::unique_ptr<std::string> __ret_;
    std::unique_ptr<BaseAST> __number_;
    // int __number_;

    friend std::ostream& 
        operator << (std::ostream & __os, const StmtAST &_a)
        {
            __os << "Stmt { " 
                << *_a.__ret_ 
                << " ";
            _a.__number_->log(__os);
            __os << " }";
            return __os;
        }
    void log(std::ostream & __os) const override{
        __os << *this;
    }
};

class NumberAST : public BaseAST {
public:
    int __int_const_;
    friend std::ostream& 
        operator << (std::ostream & __os, const NumberAST &_a)
        {
            __os << _a.__int_const_;
            return __os;
        }
    void log(std::ostream & __os) const override{
        __os << *this;
    }
};

// ...


#endif 