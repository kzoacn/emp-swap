#ifndef REAL_H__
#define REAL_H__

#include "emp-tool/emp-tool.h"
#include "emp-swap/number.h"

namespace emp{
//Real is fix-point number
#ifndef EXP
#define EXP 12
#endif

#if EXP%2==1

#error "EXP must be even"

#endif
// value = number x 2^-EXP

class Real{ public:

    Number num;


    Real(const double &x=0){
        long long y=(long long)(x*(1<<EXP));
        num=Number(BITLENGTH,y);
    }

	Real operator+(const Real& rhs) const; 
	Real operator-(const Real& rhs) const; 
    Real operator*(const Real& rhs) const; 
    Real operator/(const Real& rhs) const; 
    Real operator-() const;
    Bit operator==(const Real& rhs) const;
    Bit operator!=(const Real& rhs) const;
    Bit operator<(const Real &rhs) const;
    Bit operator<=(const Real &rhs) const;
    Bit operator>(const Real &rhs) const;
    Bit operator>=(const Real &rhs) const;
    Real select(Bit bit,Real rhs)const;
    Real sqrt()const;

    Number ceil()const;
    Number floor()const;
    Number round()const;
};
inline Real a2r(Number n,bool semantic);
    
#include"emp-swap/real.hpp"
} 





#endif