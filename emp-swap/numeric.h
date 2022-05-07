#ifndef NUMERIC_H
#define NUMERIC_H
#include "emp-tool/emp-tool.h"

namespace emp {

const static int PROVER = 1;
const static int VERIFIER = 2;

#define MOD ((1LL<<61)-1)
#define HALFMOD (MOD>>1)
#define LOGMOD 60

#ifndef BITLENGTH

#define BITLENGTH 55

#endif
const block P=_mm_set_epi64x(MOD,MOD);


block make_delta(block delta){ 
    *((char *) &delta) |= 0x01;
    return delta;
}
__attribute__((target("sse2")))
inline block double_block(block bl) {
	const __m128i mask = _mm_set_epi32(135,1,1,1);
	__m128i tmp = _mm_srai_epi32(bl, 31);
	tmp = _mm_and_si128(tmp, mask);
	tmp = _mm_shuffle_epi32(tmp, _MM_SHUFFLE(2,1,0,3));
	bl = _mm_slli_epi32(bl, 1);
	return _mm_xor_si128(bl,tmp);
}
__attribute__((target("sse2")))
inline block xorBlocks(block x, block y) {
	return _mm_xor_si128(x,y);
}
__attribute__((target("sse2")))
inline block andBlocks(block x, block y) {
	return _mm_and_si128(x,y);
}


void printBlock(block var)  {
    int64_t *v64val = (int64_t*) &var;
    printf("%.16llx %.16llx\n", (long long)v64val[1], (long long)v64val[0]);
}

inline void modBlock(block &x) {
    x=_mm_add_epi64(_mm_and_si128(x,P),_mm_srli_epi64(x,61));
    x=_mm_sub_epi64(x,_mm_andnot_si128(_mm_cmpgt_epi64(P,x),P));
}

inline void modBlock_one(block &x) {
    x=_mm_sub_epi64(x,_mm_andnot_si128(_mm_cmpgt_epi64(P,x),P));
}

inline block addBlocks(const block &x,const block &y) {
    block res=_mm_add_epi64(x,y);
    modBlock_one(res);
    return res;
}

inline block subBlocks(const block &x,const block &y) {
    block res=_mm_sub_epi64(P,y);
    return addBlocks(x,res);
}

inline block addCBlocks(const block &x,long long y) {
    return addBlocks(x,_mm_set_epi64x(y,y));
}

long long get_val(const block &val) {
    long long *vv=(long long*)&val;
    return *vv;
}

}
#endif
