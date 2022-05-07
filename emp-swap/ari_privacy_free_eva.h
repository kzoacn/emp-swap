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
				prg2.random_block(&powerOf2[0].mask, 1);
				modBlock(powerOf2[0].mask);
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
        c.mask=addBlocks(a.mask,b.mask);
        c.val=addBlocks(a.val,b.val);
	}
	void sub_gate(Number &c,const Number &a,const Number &b){
        c.mask=subBlocks(a.mask,b.mask);
        c.val=subBlocks(a.val,b.val);
	}
	void sel_gate(Number &z,const Bit &b,const Number &x,const Number &y){
		
		block diff=subBlocks(x.mask,y.mask);
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
		}
	}


	void sels_gate(int length,Number *c,const Bit *bits,const Number *a,const Number *b){
		
		if(length*sizeof(block)>buffer.size())
			buffer.resize(length*sizeof(block));
		//block *h=(block*)buffer.data();
		block h[LOGMOD];
		hash_with_indices(length,h,(block*)bits,gid,&prp.aes);
		gid+=length;

		for(int i=0;i<length;i++){
			block diff=subBlocks(a[i].mask,b[i].mask);
			modBlock(h[i]);
			block R;
			io->recv_block(&R,1);
			//printBlock(R);
			recv_h.put(&R,sizeof(block));
			c[i].mask=subBlocks(b[i].mask,h[i]);

			if(getLSB(bits[i].bit)){
				c[i].mask=addBlocks(c[i].mask,R);
				c[i].mask=addBlocks(c[i].mask,diff);
				c[i].val=a[i].val;
			}else{
				c[i].val=b[i].val;
			}
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
		eq_hash.put(&bit.bit,sizeof(block)); 

		return true;
	}
};
}
#endif// PRIVACY_FREE_EVA_H__
