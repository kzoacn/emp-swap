#ifndef ARI_PLAIN_EVA_H__
#define ARI_PLAIN_EVA_H__
#include "emp-tool/emp-tool.h"
#include "emp-swap/emp-swap.h"
#include <iostream>
namespace emp {
    
class AriPlainEva  :public ArithmeticExecution{ public:
	int64_t gid = 0;
    /*
        FOR BOOLEAN
    */
	block constant[2];
	Number powerOf2[LOGMOD];
	Number zeros[LOGMOD];
	AriPlainEva(){
		PRG prg2(&zero_block);prg2.random_block(constant, 2);
		*((char *) &constant[0]) &= 0xfe;
       	*((char *) &constant[1]) |= 0x01;
		for(int i=0;i<LOGMOD;i++){
			zeros[i]=zero_block;
			if(i==0){
				powerOf2[0].val=makeBlock(0,1);
			}else{
				add_gate(powerOf2[i],powerOf2[i-1],powerOf2[i-1]);
			}
		}
	}
	bool is_public(const block & b, int party) {
		return false;
	}
	block public_label(bool b) {
		return constant[b];
	}
	block and_gate(const block& a, const block& b) {
		return andBlocks(a,b);
	}
	block xor_gate(const block& a, const block& b) {
		return xorBlocks(a,b);
	}
	block not_gate(const block& a) {
		return xor_gate(a, public_label(true));
	}
    /*
        FOR ARITHMETIC
    */

	void add_gate(Number &c,const Number &a,const Number &b){
		c.val=addBlocks(a.val,b.val);
	}
	void sub_gate(Number &c,const Number &a,const Number &b){
		c.val=subBlocks(a.val,b.val);
	}

	void sel_gate(Number &z,const Bit &b,const Number &x,const Number &y){
		if(getLSB(b.bit)){
			z.val=x.val;
		}else{
			z.val=y.val;
		}
	}


	void sels_gate(int length,Number *c,const Bit *bits,const Number *a,const Number *b){
		for(int i=0;i<length;i++)
			sel_gate(c[i],bits[i],a[i],b[i]);
	}
	Number b2a_gate(const Integer &x){
		Number res[LOGMOD];
		powerOf2[x.size()-1]=-powerOf2[x.size()-1];
		sels_gate(x.size(),res,x.bits.data(),powerOf2,zeros);
		powerOf2[x.size()-1]=-powerOf2[x.size()-1];
		Number c=zero_block;
		for(int i=0;i<x.size();i++)
			c=c+res[i];
		return c;
	}


	Integer a2b_gate(int length,const Number &v){
		long long vv=get_val(v.val);
		return Integer(length,vv>=HALFMOD ? vv-MOD : vv,PROVER);
	}

	bool eq(const Number &a,const Number &b){
		
		return true;
	}
	bool is_true(const Bit &bit){

		return true;
	}
    
};
}
#endif// PRIVACY_FREE_EVA_H__
