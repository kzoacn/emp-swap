#ifndef ZK_NUMBER_H__
#define ZK_NUMBER_H__
#include "emp-tool/emp-tool.h"
#include "emp-swap/arithmetic_execution.h"
#include "emp-swap/numeric.h"
namespace emp {
class Number{ public:

	BIGNUM *mask,*val;

	Number() {
        mask=BN_new();
        val=BN_new();
        BN_zero(mask);
        BN_zero(val);
	}
    Number(const long long &x);
    Number(int length, long long input, int party);
    Number(const Number &oth);

    Number& operator=(const Number &rhs);

	Number operator+(const Number& rhs) const; 
	Number operator-(const Number& rhs) const;  
    Number operator*(const Integer& rhs) const;
    
    void assert_eq(const Number &rhs) const;

    Number operator-() const; 
    Number select(Bit bit,Number rhs)const; 
};
inline Number b2a(Integer x); 
#include "emp-swap/number.hpp"
}
#endif
