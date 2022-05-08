#ifndef ARITHMETIC_EXECUTION_H__
#define ARITHMETIC_EXECUTION_H__
#include "emp-tool/emp-tool.h"

namespace emp {
class Number;
class ArithmeticExecution: virtual public CircuitExecution { 
public:
	static __thread ArithmeticExecution * ari_exec;
    static block _;
	virtual void add_gate(Number &c,const Number &a,const Number &b)=0;
	virtual void sub_gate(Number &c,const Number &a,const Number &b)=0;
	virtual void sel_gate(Number &z,const Bit &b,const Number &x,const Number &y)=0;
	virtual void sels_gate(int length,Number *c,const Bit *bits,const Number *a,const Number *b)=0;
	virtual Number b2a_gate(const Integer &x)=0;
	//virtual Integer a2b_gate(int length,const Number &val)=0;
	virtual bool eq(const Number &a,const Number &b)=0;
	virtual bool is_true(const Bit &bit)=0;
	virtual bool dlog(const Number &x,const EC_POINT *P)=0;
	virtual ~ArithmeticExecution (){ }
};
}
#endif