#ifndef ZK_NUMBER_H__
#define ZK_NUMBER_H__
#include "emp-tool/emp-tool.h"
#include "emp-swap/arithmetic_execution.h"
#include "emp-swap/numeric.h"
namespace emp {
class Number{ public:
	block mask;
    block val;

	Number(const block& _m=zero_block,const block &_v=zero_block) {
		memcpy(&mask, &_m, sizeof(block));
		memcpy(&val, &_v, sizeof(block));
	}
    Number(const long long &x);
    Number(int length, long long input, int party);


	Number operator+(const Number& rhs) const; 
	Number operator-(const Number& rhs) const; 
    Number operator*(const Number& rhs) const; 
    Number operator*(const Integer& rhs) const;
    Number operator/(const Number& rhs) const;  
    Number operator%(const Number& rhs) const;  
    Number operator-() const;

    Bit operator==(const Number& rhs) const;
    Bit operator!=(const Number& rhs) const;
    Bit operator<(const Number &rhs) const;
    Bit operator<=(const Number &rhs) const;
    Bit operator>(const Number &rhs) const;
    Bit operator>=(const Number &rhs) const;

    Number operator>>(int shamt) const; 
    Number operator<<(int shamt) const; 
	Number operator<<(const Number& shamt)const;
	Number operator>>(const Number& shamt)const;


	Number operator&(const Number& rhs)const;
	Number operator|(const Number& rhs)const;    
    Number select(Bit bit,Number rhs)const;
    std::pair<Number,Number> div(const Number &rhs)const;

    Number sqrt()const;
};
inline Number b2a(Integer x);
inline Integer a2b(const Number &x,int length);
#include "emp-swap/number.hpp"
}
#endif
