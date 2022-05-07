#ifndef ARI_PRIVACY_FREE_EVA_H__
#define ARI_PRIVACY_FREE_EVA_H__
#include "emp-tool/io/io_channel.h"
#include "emp-tool/io/net_io_channel.h"
#include "emp-tool/io/file_io_channel.h"
#include "emp-tool/utils/block.h"
#include "emp-tool/utils/utils.h"
#include "emp-tool/utils/prp.h"
#include "emp-tool/utils/hash.h"
#include "emp-tool/execution/circuit_execution.h"
#include "emp-swap/arithmetic_execution.h"
#include "garble_gate_privacy_free.h"
#include "emp-swap/hash_idx.h"
#include <iostream>

namespace emp {
template<typename T>
class AriPrivacyFreeEva  :public ArithmeticExecution{ public:
	PRP prp;
	T * io;
	int64_t gid = 0;
    /*
        FOR BOOLEAN
    */
	block constant[2];
	Number powerOf2[LOGMOD];
	Number zeros[LOGMOD];
	vector<char>buffer;
	Hash recv_h;
	AriPrivacyFreeEva(T * io) :io(io) {
		PRG prg2(&zero_block);prg2.random_block(constant, 2);
		*((char *) &constant[0]) &= 0xfe;
       	*((char *) &constant[1]) |= 0x01;
		eq_hash.reset();
		recv_h.reset();
		for(int i=0;i<LOGMOD;i++){
			zeros[i]=Number();
			if(i==0){
				BN_zero(powerOf2[0].mask);
				BN_one(powerOf2[0].val);
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
		block out[2], table[1];
		io->recv_block(table, 1);
		recv_h.put(table,sizeof(block));
		garble_gate_eval_privacy_free(a, b, out, table, gid++, &prp.aes);
		return out[0];
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
		BN_mod_add(c.mask,a.mask,b.mask,MOD,CTX);
		BN_mod_add(c.val,a.val,b.val,MOD,CTX);
	}
	void sub_gate(Number &c,const Number &a,const Number &b){
		BN_mod_sub(c.mask,a.mask,b.mask,MOD,CTX);
		BN_mod_sub(c.val,a.val,b.val,MOD,CTX);
	}
	void sel_gate(Number &z,const Bit &b,const Number &x,const Number &y){
		
		/*block diff=subBlocks(x.mask,y.mask);
		block h=hash_with_idx(b.bit,gid++,&prp.aes);
		modBlock(h);
		block R;
		io->recv_block(&R,1);
		recv_h.put(&R,sizeof(block));
		z.mask=subBlocks(y.mask,h);

		if(getLSB(b.bit)){
			z.mask=addBlocks(z.mask,R);
			z.mask=addBlocks(z.mask,diff);
			z.val=x.val;
		}else{
			z.val=y.val;
		}*/

		BIGNUM *diff,*h,*R;
		diff=BN_new();
		h=BN_new();
		R=BN_new();

		block h0[2];
		h0[0]=hash_with_idx(b.bit,gid,&prp.aes);
		h0[1]=hash_with_idx(b.bit,gid+1,&prp.aes);

		gid+=2;

		BN_bin2bn((const unsigned char*)&h0[0],32,h);

		int size;
		unsigned char tmp[32];
		io->recv_data(&size,4);
		io->recv_data(tmp,32);
		BN_bin2bn(tmp,size,R);
		recv_h.put(tmp,size);
		BN_mod_sub(z.mask,y.mask,h,MOD,CTX);
		if(getLSB(b.bit)){
			BN_mod_add(z.mask,z.mask,R,MOD,CTX);
			BN_mod_add(z.mask,z.mask,diff,MOD,CTX);
			BN_copy(z.val,x.val);
		}else{
			BN_copy(z.val,y.val);
		}

		BN_free(diff);
		BN_free(h);
		BN_free(R);
	}


	void sels_gate(int length,Number *c,const Bit *bits,const Number *a,const Number *b){
		for(int i=0;i<length;i++)
			sel_gate(c[i],bits[i],a[i],b[i]);
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
		eq_hash.put(&bit.bit,sizeof(block)); 

		return true;
	}
};
}
#endif// PRIVACY_FREE_EVA_H__
