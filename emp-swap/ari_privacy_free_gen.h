#ifndef ARI_PRIVACY_FREE_GEN_H__
#define ARI_PRIVACY_FREE_GEN_H__
#include "emp-tool/emp-tool.h"
#include "emp-swap/numeric.h"
#include "emp-swap/hash_idx.h"
#include "garble_gate_privacy_free.h"
#include <iostream>

namespace emp {
template<typename T>
class AriPrivacyFreeGen: public ArithmeticExecution{ 
public:
	PRP prp;
	T * io;
	int64_t gid = 0;
	AriPrivacyFreeGen(T * io) :io(io) {
		PRG tmp;
		block a,b;
		tmp.random_block(&a, 1);
		*((char *) &a) |= 0x01;
		tmp.random_block(&b, 1);
		set_delta(a,b);
		eq_hash.reset();
	}
    /*
     FOR BOOLEAN
    */
	block constant[2];
	Number powerOf2[LOGMOD];
	Number zeros[LOGMOD];
	block delta;
	vector<char>buffer;
	bool is_public(const block & b, int party) {
		return false;
	}
	bool isDelta(const block & b) {
		__m128i neq = _mm_xor_si128(b, delta);
		return _mm_testz_si128(neq, neq);
	}
	void set_delta(const block &_delta,const block &_mdelta) {
		this->delta = make_delta(_delta);
		this->mdelta = _mdelta;
		modBlock(this->mdelta);
		PRG prg2(&zero_block);prg2.random_block(constant, 2);
		*((char *) &constant[0]) &= 0xfe;
		*((char *) &constant[1]) |= 0x01;
		constant[1]=xorBlocks(constant[1],delta);

		for(int i=0;i<LOGMOD;i++){
			zeros[i]=Number();
			if(i==0){
				prg2.random_block(&powerOf2[0].mask, 1);
				modBlock(powerOf2[0].mask);
				powerOf2[0].mask=subBlocks(powerOf2[0].mask,mdelta);
			}else{
				add_gate(powerOf2[i],powerOf2[i-1],powerOf2[i-1]);
			}
		}
	}
	block public_label(bool b) {
		return constant[b];
	}
	block and_gate(const block& a, const block& b) {
		block out[2], table[2];
		garble_gate_garble_privacy_free(a, xorBlocks(a,delta), b, xorBlocks(b,delta), 
				&out[0], &out[1], delta, table, gid++, &prp.aes);
		io->send_block(table, 1);
		return out[0];
	}
	block xor_gate(const block&a, const block& b) {
		return xorBlocks(a, b);
	}
	block not_gate(const block& a) {
		return xor_gate(a, public_label(true));
	}

    /*
     FOR ARITHMETIC
    */
	block mdelta;
	block delta2k[LOGMOD];
	void add_gate(Number &c,const Number &a,const Number &b){
        c.mask=addBlocks(a.mask,b.mask);
	}
	void sub_gate(Number &c,const Number &a,const Number &b){
        c.mask=subBlocks(a.mask,b.mask);
	}	

	void sel_gate(Number &z,const Bit &b,const Number &x,const Number &y){
		block diff=subBlocks(x.mask,y.mask);
//xiao: merge to one call?
//TODO
		block h0=hash_with_idx(b.bit,gid,&prp.aes);
		block h1=hash_with_idx(xorBlocks(b.bit,delta),gid,&prp.aes);
		gid++;
		modBlock(h0);
		modBlock(h1);
		block R=subBlocks(h1,h0);
		R=subBlocks(R,diff);
		io->send_block(&R,1);
		z.mask=subBlocks(y.mask,h0);
	}

	void sels_gate(int length,Number *c,const Bit *bits,const Number *a,const Number *b){
		if(length*2*sizeof(block)>buffer.size())
			buffer.resize(length*2*sizeof(block));
		block *h=(block*)buffer.data();
		block *h0,*h1;
		
		hash_with_delta(length,h,(block*)bits,gid,delta,&prp.aes);
		gid+=length;

		h0=h;
		h1=h+length;

		for(int i=0;i<length;i++){	
			block diff=subBlocks(a[i].mask,b[i].mask);
			modBlock(h0[i]);
			modBlock(h1[i]);
			block R=subBlocks(h1[i],h0[i]);
			R=subBlocks(R,diff);
			io->send_block(&R,1);
			c[i].mask=subBlocks(b[i].mask,h0[i]);
		}

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

	Hash eq_hash;
	bool eq(const Number &a,const Number &b){
		Number diff=a-b;
		eq_hash.put(&diff.mask,sizeof(block));
		return true;
	}
	bool is_true(const Bit &bit){
		block tmp=xorBlocks(bit.bit,delta); 
		eq_hash.put(&tmp,sizeof(block));
		return true;
	}
};
}


#endif// PRIVACY_FREE_GEN_H__
