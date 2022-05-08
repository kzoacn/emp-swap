#ifndef ARI_PRIVACY_FREE_GEN_H__
#define ARI_PRIVACY_FREE_GEN_H__
#include "emp-tool/emp-tool.h"
#include "emp-swap/numeric.h"
#include "emp-swap/hash_idx.h"
#include "garble_gate_privacy_free.h"
#include "emp-swap/constant.h"
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
		block a,b[2];
		tmp.random_block(&a, 1);
		*((char *) &a) |= 0x01;
		tmp.random_block(b, 2);

		BIGNUM *mdelta=BN_new(); 
		//BN_bin2bn((const unsigned char *) &b[0], 32, mdelta);
		BN_one(mdelta);
		BN_mod(mdelta, mdelta, MOD,CTX);

		set_delta(a,mdelta);
		BN_free(mdelta);
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
	void set_delta(const block &_delta,const BIGNUM *_mdelta) {
		this->delta = make_delta(_delta);
		this->mdelta=BN_new();
		BN_copy(this->mdelta,_mdelta); 
		PRG prg2(&zero_block);prg2.random_block(constant, 2);
		*((char *) &constant[0]) &= 0xfe;
		*((char *) &constant[1]) |= 0x01;
		constant[1]=xorBlocks(constant[1],delta);

		for(int i=0;i<LOGMOD;i++){
			zeros[i]=Number();
			if(i==0){
				BN_one(powerOf2[0].mask);
				BN_mod_sub(powerOf2[0].mask,powerOf2[0].mask,mdelta,MOD,CTX);
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
	BIGNUM *mdelta;
	BIGNUM *delta2k[LOGMOD];
	void add_gate(Number &c,const Number &a,const Number &b){
        //c.mask=addBlocks(a.mask,b.mask);
		BN_mod_add(c.mask,a.mask,b.mask,MOD,CTX);
	}
	void sub_gate(Number &c,const Number &a,const Number &b){
        //c.mask=subBlocks(a.mask,b.mask);
		BN_mod_sub(c.mask,a.mask,b.mask,MOD,CTX);
	}	

	void sel_gate(Number &z,const Bit &b,const Number &x,const Number &y){
		/*block diff=subBlocks(x.mask,y.mask); 
		block h0=hash_with_idx(b.bit,gid,&prp.aes);
		block h1=hash_with_idx(xorBlocks(b.bit,delta),gid,&prp.aes);
		gid++;
		modBlock(h0);
		modBlock(h1);
		block R=subBlocks(h1,h0);
		R=subBlocks(R,diff);
		io->send_block(&R,1);
		z.mask=subBlocks(y.mask,h0);*/

		BIGNUM *diff,*h0,*h1,*R;

		diff=BN_new();
		h0=BN_new();
		h1=BN_new();
		R=BN_new();
 
		BN_mod_sub(diff,x.mask,y.mask,MOD,CTX);

		block h[2][2];
		unsigned char tmp[32];
		h[0][0]=hash_with_idx(b.bit,gid,&prp.aes);
		h[0][1]=hash_with_idx(b.bit,gid+1,&prp.aes);
		h[1][0]=hash_with_idx(xorBlocks(b.bit,delta),gid,&prp.aes);
		h[1][1]=hash_with_idx(xorBlocks(b.bit,delta),gid+1,&prp.aes);

		
		gid+=2;
		
		BN_bin2bn((const unsigned char*) &h[0][0],32,h0);
		BN_bin2bn((const unsigned char*) &h[1][0],32,h1);
		
		BN_mod(h0,h0,MOD,CTX); 
		BN_mod(h1,h1,MOD,CTX);
 
		BN_mod_sub(R,h1,h0,MOD,CTX);
		BN_mod_sub(R,R,diff,MOD,CTX);

		int size;
		size = BN_num_bytes(R);
		BN_bn2bin(R,tmp);
		 

		io->send_data(&size,4);
		io->send_data(tmp,32);
		BN_mod_sub(z.mask,y.mask,h0,MOD,CTX);
		
		BIGNUM *t=BN_new();
		BN_mod_add(t,z.mask,mdelta,MOD,CTX);	  

		BN_free(diff);
		BN_free(h0);
		BN_free(h1);
		BN_free(R);
	}

	void sels_gate(int length,Number *c,const Bit *bits,const Number *a,const Number *b){
		for(int i=0;i<length;i++){ 
			sel_gate(c[i],bits[i],a[i],b[i]);
		}
	}
	Number b2a_gate(const Integer &x){
		Number res[LOGMOD];
		sels_gate(x.size(),res,x.bits.data(),powerOf2,zeros);
		Number c;
		for(int i=0;i<x.size();i++)
			c=c+res[i];
		return c;
	}

	Hash eq_hash;
	bool eq(const Number &a,const Number &b){
		Number diff=a-b; 

		unsigned char tmp[32];
		memset(tmp,0,sizeof(tmp));
		BN_bn2bin(diff.mask,tmp);
		eq_hash.put(tmp,sizeof(tmp));
		return true;
	}
	bool is_true(const Bit &bit){
		block tmp=xorBlocks(bit.bit,delta); 
		eq_hash.put(&tmp,sizeof(block));
		return true;
	}
	bool dlog(const Number &x,const EC_POINT *P){
		EC_POINT *Q=EC_POINT_new(GROUP);
		EC_POINT_mul(GROUP,Q,x.mask,P,mdelta,CTX);

		int size = EC_POINT_point2oct(GROUP, Q, POINT_CONVERSION_UNCOMPRESSED, NULL, 0, CTX);
		unsigned char *tmp=new unsigned char[size];
		EC_POINT_point2oct(GROUP, Q, POINT_CONVERSION_UNCOMPRESSED, tmp, size, CTX);
		eq_hash.put(tmp,size); 
		delete[] tmp;

		return true;
	}
};
}


#endif// PRIVACY_FREE_GEN_H__
